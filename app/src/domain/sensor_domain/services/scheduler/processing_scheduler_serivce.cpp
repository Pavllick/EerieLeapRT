#include <stdio.h>
#include <vector>
#include <zephyr/sys/util.h>

#include "utilities/time/time_helpers.hpp"
#include "zephyr/kernel.h"
#include "processing_scheduler_serivce.h"

namespace eerie_leap::domain::sensor_domain::services::scheduler {

using namespace eerie_leap::domain::sensor_domain::models;

LOG_MODULE_REGISTER(processing_scheduler_logger);

ProcessingSchedulerService::ProcessingSchedulerService(
    std::shared_ptr<ITimeService> time_service,
    std::shared_ptr<GuidGenerator> guid_generator,
    std::shared_ptr<IAdc> adc,
    std::shared_ptr<IGpio> gpio,
    std::shared_ptr<SensorsConfigurationController> sensors_configuration_controller)
    : time_service_(std::move(time_service)),
    guid_generator_(std::move(guid_generator)),
    adc_(std::move(adc)),
    gpio_(std::move(gpio)),
    sensors_configuration_controller_(std::move(sensors_configuration_controller)) {

    k_sem_init(&sync_semaphore_, 0, 1);
    k_sem_init(&sensors_processing_semaphore_, 1, 1);

    sensor_readings_frame_ = std::make_shared<SensorReadingsFrame>();
    sensor_reader_ = std::make_shared<SensorReader>(time_service_, guid_generator_, adc_, sensor_readings_frame_);
    sensor_processor_ = std::make_shared<SensorProcessor>(sensor_readings_frame_);

    indicator_readings_frame_ = std::make_shared<IndicatorReadingsFrame>();
    indicator_reader_ = std::make_shared<IndicatorReader>(time_service_, guid_generator_, gpio_, indicator_readings_frame_);
    indicator_processor_ = std::make_shared<IndicatorProcessor>(indicator_readings_frame_);
};

void ProcessingSchedulerService::ProcessSensorWorkTask(k_work* work) {
    SensorTask* task = CONTAINER_OF(work, SensorTask, work);

    if(k_sem_take(task->processing_semaphore, PROCESSING_TIMEOUT) == 0) {
        try {
            task->reader->Read(task->sensor);
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

void ProcessingSchedulerService::ProcessIndicatorWorkTask(k_work* work) {
    IndicatorTask* task = CONTAINER_OF(work, IndicatorTask, work);

    if(k_sem_take(task->processing_semaphore, PROCESSING_TIMEOUT) == 0) {
        try {
            task->reader->Read(task->sensor);
            task->processor->ProcessReading(task->readings_frame->GetReading(task->sensor->id));

            auto reading = task->readings_frame->GetReading(task->sensor->id);

            printf("Indicator Reading - ID: %s, Guid: %llu, Value: %d, Time: %s\n",
                task->sensor->id.c_str(),
                reading->id.AsUint64(),
                reading->value.value_or(false),
                TimeHelpers::GetFormattedString(*reading->timestamp).c_str());
        } catch (const std::exception& e) {
            LOG_ERR("Error processing indicator sensor: %s, Error: %s", task->sensor->id.c_str(), e.what());
        }
    } else {
        LOG_ERR("Lock take timed out for sensor: %s", task->sensor->id.c_str());
    }

    k_sem_give(task->processing_semaphore);
    k_work_reschedule(&task->work, task->sampling_rate_ms);
}

std::shared_ptr<SensorTask> ProcessingSchedulerService::CreateSensorTask(std::shared_ptr<Sensor> sensor) {
    auto task = std::make_shared<SensorTask>();
    task->processing_semaphore = &sensors_processing_semaphore_;
    task->sampling_rate_ms = K_MSEC(sensor->configuration.sampling_rate_ms);
    task->sensor = sensor;
    task->readings_frame = sensor_readings_frame_;
    task->reader = sensor_reader_;
    task->processor = sensor_processor_;

    return task;
}

std::shared_ptr<IndicatorTask> ProcessingSchedulerService::CreateIndicatorTask(std::shared_ptr<Sensor> sensor) {
    auto task = std::make_shared<IndicatorTask>();
    task->processing_semaphore = &sensors_processing_semaphore_;
    task->sampling_rate_ms = K_MSEC(sensor->configuration.sampling_rate_ms);
    task->sensor = sensor;
    task->readings_frame = indicator_readings_frame_;
    task->reader = indicator_reader_;
    task->processor = indicator_processor_;

    return task;
}

void ProcessingSchedulerService::Start() {
    auto sensors = sensors_configuration_controller_->Get();

    // Required to keep SensorTasks in scope until the end of the function
    std::vector<std::shared_ptr<SensorTask>> sensor_tasks;
    std::vector<std::shared_ptr<IndicatorTask>> indicator_tasks;

    for(const auto& sensor : *sensors) {
        k_work_delayable* work;

        if(sensor->configuration.type == SensorType::PHYSICAL_ANALOG || sensor->configuration.type == SensorType::VIRTUAL_ANALOG) {
            auto task = CreateSensorTask(sensor);
            sensor_tasks.push_back(task);

            work = &task->work;
            k_work_init_delayable(work, ProcessSensorWorkTask);
        } else if(sensor->configuration.type == SensorType::PHYSICAL_INDICATOR || sensor->configuration.type == SensorType::VIRTUAL_INDICATOR) {
            auto task = CreateIndicatorTask(sensor);
            indicator_tasks.push_back(task);

            work = &task->work;
            k_work_init_delayable(work, ProcessIndicatorWorkTask);
        } else {
            throw std::runtime_error("Unsupported sensor type");
        }

        k_work_schedule(work, K_NO_WAIT);
    }

    (void)k_sem_take(&sync_semaphore_, K_FOREVER);
}

} // namespace eerie_leap::domain::sensor_domain::services::scheduler
