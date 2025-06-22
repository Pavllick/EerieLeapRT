#include <vector>
#include <zephyr/sys/util.h>
#include <zephyr/kernel.h>

#include "utilities/time/time_helpers.hpp"
#include "utilities/memory/heap_allocator.h"
#include "processing_scheduler_serivce.h"

namespace eerie_leap::domain::sensor_domain::services {

using namespace eerie_leap::utilities::memory;
using namespace eerie_leap::domain::sensor_domain::models;

LOG_MODULE_REGISTER(processing_scheduler_logger);

ProcessingSchedulerService::ProcessingSchedulerService(
    std::shared_ptr<ITimeService> time_service,
    std::shared_ptr<GuidGenerator> guid_generator,
    std::shared_ptr<IAdcManager> adc_manager,
    std::shared_ptr<IGpio> gpio,
    std::shared_ptr<AdcConfigurationController> adc_configuration_controller,
    std::shared_ptr<SensorsConfigurationController> sensors_configuration_controller)
    : time_service_(std::move(time_service)),
    guid_generator_(std::move(guid_generator)),
    adc_manager_(std::move(adc_manager)),
    gpio_(std::move(gpio)),
    adc_configuration_controller_(std::move(adc_configuration_controller)),
    sensors_configuration_controller_(std::move(sensors_configuration_controller)) {

    k_sem_init(&processing_semaphore_, 1, 1);
};

void ProcessingSchedulerService::ProcessSensorWorkTask(k_work* work) {
    SensorTask* task = CONTAINER_OF(work, SensorTask, work);

    if(k_sem_take(task->processing_semaphore, PROCESSING_TIMEOUT) == 0) {
        try {
            task->reader->Read();
            task->processor->ProcessReading(task->readings_frame->GetReading(task->sensor->id));

            auto reading = task->readings_frame->GetReading(task->sensor->id);

            printf("Sensor Reading - ID: %s, Guid: %llu, Value: %.3f, Time: %s\n",
                task->sensor->id.c_str(),
                reading->id.AsUint64(),
                reading->value.value_or(0.0f),
                TimeHelpers::GetFormattedString(*reading->timestamp).c_str());
        } catch (const std::exception& e) {
            LOG_ERR("Error processing sensor: %s, Error: %s", task->sensor->id.c_str(), e.what());
        }
    } else {
        LOG_ERR("Lock take timed out for sensor: %s", task->sensor->id.c_str());
    }

    k_sem_give(task->processing_semaphore);
    k_work_reschedule(&task->work, task->sampling_rate_ms);
}

std::shared_ptr<SensorTask> ProcessingSchedulerService::CreateSensorTask(std::shared_ptr<Sensor> sensor) {
    auto task = make_shared_ext<SensorTask>();
    task->processing_semaphore = &processing_semaphore_;
    task->sampling_rate_ms = K_MSEC(sensor->configuration.sampling_rate_ms);
    task->sensor = sensor;
    task->readings_frame = sensor_readings_frame_;

    auto sensor_reader = make_shared_ext<SensorReader>(
        time_service_,
        guid_generator_,
        adc_configuration_controller_,
        gpio_,
        sensor_readings_frame_,
        sensor);
    task->reader = sensor_reader;

    task->processor = sensor_processor_;

    return task;
}

void ProcessingSchedulerService::StartTasks() {
    for(auto task : sensor_tasks_) {
        k_work_delayable* work = &task->work;
        k_work_init_delayable(work, ProcessSensorWorkTask);

        k_work_schedule(work, K_NO_WAIT);
    }

    k_sleep(K_MSEC(1));
}

void ProcessingSchedulerService::Start() {
    sensor_readings_frame_ = make_shared_ext<SensorReadingsFrame>();

    sensor_processor_ = make_shared_ext<SensorProcessor>(sensor_readings_frame_);

    auto sensors = sensors_configuration_controller_->Get();

    for(const auto& sensor : *sensors) {
        LOG_INF("Creating task for sensor: %s", sensor->id.c_str());
        auto task = CreateSensorTask(sensor);
        sensor_tasks_.push_back(task);
    }

    StartTasks();

    LOG_INF("Processing Scheduler Service started");
}

void ProcessingSchedulerService::Restart() {
    struct k_work_sync sync;

    while(sensor_tasks_.size() > 0) {
        for(int i = 0; i < sensor_tasks_.size(); i++) {
            LOG_INF("Canceling task for sensor: %s", sensor_tasks_[i]->sensor->id.c_str());
            bool res = k_work_cancel_delayable_sync(&sensor_tasks_[i]->work, &sync);
            if(!res) {
                sensor_tasks_.erase(sensor_tasks_.begin() + i);
                break;
            }
        }
    }

    LOG_INF("Processing Scheduler Service stopped");

    sensor_tasks_.clear();
    Start();
}

void ProcessingSchedulerService::Pause() {
    struct k_work_sync sync;
    std::vector<std::shared_ptr<SensorTask>> sensor_tasks_temp;
    for(auto task : sensor_tasks_)
        sensor_tasks_temp.push_back(task);

    while(sensor_tasks_temp.size() > 0) {
        for(int i = 0; i < sensor_tasks_temp.size(); i++) {
            LOG_INF("Canceling task for sensor: %s", sensor_tasks_temp[i]->sensor->id.c_str());
            bool res = k_work_cancel_delayable_sync(&sensor_tasks_temp[i]->work, &sync);
            if(!res) {
                sensor_tasks_temp.erase(sensor_tasks_temp.begin() + i);
                break;
            }
        }
    }

    LOG_INF("Processing Scheduler Service stopped");

    sensor_tasks_temp.clear();
}

void ProcessingSchedulerService::Resume() {
    StartTasks();
}

} // namespace eerie_leap::domain::sensor_domain::services
