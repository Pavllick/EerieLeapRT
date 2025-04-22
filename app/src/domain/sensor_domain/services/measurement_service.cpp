#include <vector>
#include <stdio.h>
#include <zephyr/logging/log.h>

#include "measurement_service.h"
#include "domain/adc_domain/hardware/adc.h"
#include "domain/sensor_domain/models/sensor.h"
#include "domain/sensor_domain/models/sensor_reading.h"
#include "domain/sensor_domain/events/sensor_reading_event.h"
#include "utilities/math_parser/expression_evaluator.h"
#include "utilities/dev_tools/system_info.h"

namespace eerie_leap::domain::sensor_domain::services {

using namespace eerie_leap::domain::adc_domain::hardware;
using namespace eerie_leap::domain::sensor_domain::models;
using namespace eerie_leap::utilities::math_parser;
using namespace eerie_leap::utilities::dev_tools;

LOG_MODULE_REGISTER(measurement_service_logger);

k_tid_t MeasurementService::Start() {
    thread_id_ = k_thread_create(
        &thread_data_,
        stack_area_,
        K_THREAD_STACK_SIZEOF(stack_area_),
        ThreadTrampoline,
        this, NULL, NULL,
        kPriority, 0, K_NO_WAIT);

    return thread_id_;
}

void MeasurementService::ThreadTrampoline(void* instance, void* p2, void* p3) {
    static_cast<MeasurementService*>(instance)->EntryPoint();
}

void MeasurementService::EntryPoint() {
    LOG_INF("Measurement Service started");

    k_mutex_init(&sensors_reading_mutex_);
    
    Adc adc;
    adc.UpdateConfiguration(AdcConfig{
        .channel_count = 8,
        .resolution = 12,
        .reference_voltage = 3.3
    });
    adc.Initialize();

    sensors_configuration_service_->Initialize();

    sensor_readings_frame_ = std::make_shared<SensorReadingsFrame>();
    sensors_reader_ = std::make_shared<SensorsReader>(adc, sensor_readings_frame_);
    sensor_processor_ = std::make_shared<SensorProcessor>(sensor_readings_frame_);

    while (true) {
        ProcessSensorsReading();
        k_msleep(READING_INTERVAL_MS_);
    }
    
    return;
}

void MeasurementService::ProcessSensorsReading() {
    k_mutex_lock(&sensors_reading_mutex_, K_FOREVER);

    sensor_readings_frame_->ClearReadings();
    
    auto sensors = sensors_configuration_service_->GetSensors();
    sensors_reader_->ReadSensors(sensors);

    for(const auto& sensor : sensors)
        sensor_processor_->ProcessSensorReading(*sensor_readings_frame_->GetReading(sensor.id));

    // for (const auto& reading : sensor_readings_frame_->GetReadings())
    //     printf("Sensor ID: %s, Value: %f\n", reading.first.c_str(), reading.second->value.value());
        // LOG_INF("Sensor ID: %s, Value: %f", reading.first.c_str(), reading.second->value.value());

    k_mutex_unlock(&sensors_reading_mutex_);
}

} // namespace eerie_leap::domain::sensor_domain::services