#include <vector>
#include <zephyr/sys/util.h>
#include <zephyr/kernel.h>

#include "utilities/time/time_helpers.hpp"
#include "utilities/memory/heap_allocator.h"
#include "domain/sensor_domain/models/sensor_type.h"
#include "domain/sensor_domain/processors/sensor_reader/i_sensor_reader.h"
#include "domain/sensor_domain/processors/sensor_reader/sensor_reader_physical_analog.h"
#include "domain/sensor_domain/processors/sensor_reader/sensor_reader_physical_indicator.h"
#include "domain/sensor_domain/processors/sensor_reader/sensor_reader_virtual_analog.h"
#include "domain/sensor_domain/processors/sensor_reader/sensor_reader_virtual_indicator.h"
#include "processing_scheduler_service.h"

namespace eerie_leap::domain::sensor_domain::services {

using namespace eerie_leap::utilities::memory;
using namespace eerie_leap::domain::sensor_domain::models;
using namespace eerie_leap::domain::sensor_domain::processors::sensor_reader;

LOG_MODULE_REGISTER(processing_scheduler_logger);

ProcessingSchedulerService::ProcessingSchedulerService(
    std::shared_ptr<ITimeService> time_service,
    std::shared_ptr<GuidGenerator> guid_generator,
    std::shared_ptr<IGpio> gpio,
    std::shared_ptr<AdcConfigurationController> adc_configuration_controller,
    std::shared_ptr<SensorsConfigurationController> sensors_configuration_controller,
    std::shared_ptr<SensorReadingsFrame> sensor_readings_frame)
    : time_service_(std::move(time_service)),
    guid_generator_(std::move(guid_generator)),
    gpio_(std::move(gpio)),
    adc_configuration_controller_(std::move(adc_configuration_controller)),
    sensors_configuration_controller_(std::move(sensors_configuration_controller)),
    sensor_readings_frame_(std::move(sensor_readings_frame)),
    reading_processors_(std::make_shared<std::vector<std::shared_ptr<IReadingProcessor>>>()) {

    k_sem_init(&processing_semaphore_, 1, 1);
};

void ProcessingSchedulerService::ProcessSensorWorkTask(k_work* work) {
    SensorTask* task = CONTAINER_OF(work, SensorTask, work);

    if(k_sem_take(task->processing_semaphore, PROCESSING_TIMEOUT) == 0) {
        try {
            task->reader->Read();
            auto reading = task->readings_frame->GetReading(task->sensor->id);

            for(auto processor : *task->reading_processors)
                processor->ProcessReading(reading);

            LOG_DBG("Sensor Reading - ID: %s, Guid: %llu, Value: %.3f, Time: %s",
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
    task->reading_processors = reading_processors_;

    std::shared_ptr<ISensorReader> sensor_reader;

    if(sensor->configuration.type == SensorType::PHYSICAL_ANALOG) {
        sensor_reader = make_shared_ext<SensorReaderPhysicalAnalog>(
            time_service_,
            guid_generator_,
            sensor_readings_frame_,
            sensor,
            adc_configuration_controller_);
    } else if(sensor->configuration.type == SensorType::VIRTUAL_ANALOG) {
        sensor_reader = make_shared_ext<SensorReaderVirtualAnalog>(
            time_service_,
            guid_generator_,
            sensor_readings_frame_,
            sensor);
    } else if(sensor->configuration.type == SensorType::PHYSICAL_INDICATOR) {
        sensor_reader = make_shared_ext<SensorReaderPhysicalIndicator>(
            time_service_,
            guid_generator_,
            sensor_readings_frame_,
            sensor,
            gpio_);
    } else if(sensor->configuration.type == SensorType::VIRTUAL_INDICATOR) {
        sensor_reader = make_shared_ext<SensorReaderVirtualIndicator>(
            time_service_,
            guid_generator_,
            sensor_readings_frame_,
            sensor);
    } else {
        throw std::runtime_error("Unsupported sensor type");
    }

    task->reader = sensor_reader;

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

void ProcessingSchedulerService::RegisterReadingProcessor(std::shared_ptr<IReadingProcessor> processor) {
    reading_processors_->push_back(processor);
}

void ProcessingSchedulerService::Start() {
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
    sensor_readings_frame_->ClearReadings();
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
