#include <stdio.h>
#include <vector>
#include <zephyr/sys/util.h>

#include "domain/sensor_domain/models/sensor.h"
#include "utilities/time/time_helpers.hpp"
#include "sensor_task.hpp"
#include "processing_scheduler_serivce.h"

namespace eerie_leap::domain::sensor_domain::services::scheduler {

using namespace eerie_leap::domain::sensor_domain::models;

LOG_MODULE_REGISTER(processing_scheduler_logger);

ProcessingSchedulerService::ProcessingSchedulerService(
    std::shared_ptr<ITimeService> time_service,
    std::shared_ptr<GuidGenerator> guid_generator,
    std::shared_ptr<IAdc> adc, std::shared_ptr<SensorsConfigurationController> sensors_configuration_controller)
    : time_service_(std::move(time_service)),
    guid_generator_(std::move(guid_generator)),
    adc_(std::move(adc)),
    sensors_configuration_controller_(std::move(sensors_configuration_controller)) {

    k_sem_init(&sync_semaphore_, 0, 1);
    k_sem_init(&sensors_processing_semaphore_, 1, 1);

    sensor_readings_frame_ = std::make_shared<SensorReadingsFrame>();
    sensor_reader_ = std::make_shared<SensorReader>(time_service_, guid_generator_, adc_, sensor_readings_frame_);
    sensor_processor_ = std::make_shared<SensorProcessor>(sensor_readings_frame_);
};

void ProcessingSchedulerService::ProcessWorkTask(k_work* work) {
    SensorTask* task = CONTAINER_OF(work, SensorTask, work);

    if(k_sem_take(task->sensors_processing_semaphore, PROCESSING_TIMEOUT) == 0) {
        try {
            task->sensor_reader->Read(task->sensor);
            task->sensor_processor->ProcessSensorReading(task->sensor_readings_frame->GetReading(task->sensor->id));

            auto reading = task->sensor_readings_frame->GetReading(task->sensor->id);

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

    k_sem_give(task->sensors_processing_semaphore);
    k_work_reschedule(&task->work, task->sampling_rate_ms);
}

void ProcessingSchedulerService::Start() {
    auto sensors = sensors_configuration_controller_->Get();

    // Required to keep SensorTasks in scope until the end of the function
    std::vector<std::shared_ptr<SensorTask>> sensor_tasks;

    for(const auto& sensor : *sensors) {
        auto task = std::make_shared<SensorTask>();
        task->sensors_processing_semaphore = &sensors_processing_semaphore_;
        task->sampling_rate_ms = K_MSEC(sensor->configuration.sampling_rate_ms);
        task->sensor = sensor;
        task->sensor_readings_frame = sensor_readings_frame_;
        task->sensor_reader = sensor_reader_;
        task->sensor_processor = sensor_processor_;

        sensor_tasks.push_back(task);

        k_work_init_delayable(&task->work, ProcessWorkTask);
        k_work_schedule(&task->work, K_NO_WAIT);
    }

    (void)k_sem_take(&sync_semaphore_, K_FOREVER);
}

} // namespace eerie_leap::domain::sensor_domain::services::scheduler