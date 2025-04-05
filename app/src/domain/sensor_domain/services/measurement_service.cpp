#include <vector>
#include <stdio.h>
#include <zephyr/logging/log.h>

#include "measurement_service.h"
#include "domain/adc_domain/hardware/adc.h"
#include "domain/sensor_domain/models/sensor.h"
#include "domain/sensor_domain/models/sensor_reading.h"
#include "domain/sensor_domain/events/sensor_reading_event.h"
#include "utilities/expression_evaluator.h"
#include "domain/sensor_domain/utilities/sensors_order_resolver.h"

namespace eerie_leap::domain::sensor_domain::services {

using namespace eerie_leap::domain::adc_domain::hardware;
using namespace eerie_leap::domain::sensor_domain::models;
using namespace eerie_leap::domain::sensor_domain::utilities;
using namespace eerie_leap::utilities;

LOG_MODULE_REGISTER(measurement_service_logger);

k_tid_t MeasurementService::Start() {
    thread_id_ = k_thread_create(
        &thread_data_,
        stack_area,
        K_THREAD_STACK_SIZEOF(stack_area),
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
    
    Adc adc;
    adc.Initialize();

    Sensor s1;
    s1.id = "sensor1";
    s1.configuration.conversion_expression_sanitized = "x + {sensor2} + 1";
    s1.configuration.type = SensorType::PHYSICAL_ANALOG;

    Sensor s2;
    s2.id = "sensor2";
    s2.configuration.conversion_expression_sanitized = "(x + 6) * 2";
    s2.configuration.type = SensorType::PHYSICAL_ANALOG;

    Sensor s3;
    s3.id = "sensor3";
    s3.configuration.conversion_expression_sanitized = "x + {sensor2} + {sensor1} + {sensor4} + 1";
    s3.configuration.type = SensorType::PHYSICAL_ANALOG;

    Sensor s4;
    s4.id = "sensor4";
    s4.configuration.conversion_expression_sanitized = "{sensor2} + 8";
    s4.configuration.type = SensorType::VIRTUAL_ANALOG;

    // s2, s1, s3, s4

    // std::vector<Sensor> sensors = {s4, s3, s1, s1};

    SensorsOrderResolver resolver;
    resolver.AddSensor(s4);
    resolver.AddSensor(s3);
    resolver.AddSensor(s2);
    resolver.AddSensor(s1);
    auto res = resolver.GetProcessingOrder();

    printf("Exp %f\n", 35.0f);
}

} // namespace eerie_leap::domain::sensor_domain::services