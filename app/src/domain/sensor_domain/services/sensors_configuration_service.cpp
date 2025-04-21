#include "sensors_configuration_service.h"
#include "domain/sensor_domain/utilities/sensors_order_resolver.h"
#include "domain/sensor_domain/models/calibration_data.h"
#include "utilities/math_parser/expression_evaluator.h"

namespace eerie_leap::domain::sensor_domain::services {

using namespace eerie_leap::domain::sensor_domain::models;
using namespace eerie_leap::domain::sensor_domain::utilities;
using namespace eerie_leap::utilities::math_parser;

SensorsConfigurationService::SensorsConfigurationService() {
    // Test Sensors

    CalibrationData calibration_data_1 {
        .min_voltage = 0.0,
        .max_voltage = 3.3,
        .min_value = 0.0,
        .max_value = 100.0
    };

    ExpressionEvaluator expression_evaluator_1("x * 2 + 1");

    Sensor sensor_1 {
        .id = "sensor_1",
        .metadata = {
            .name = "Sensor 1",
            .unit = "km/h",
            .description = "Test Sensor 1"
        },
        .configuration = {
            .type = SensorType::PHYSICAL_ANALOG,
            .channel = 0,
            .calibration = calibration_data_1,
            .expression_evaluator = expression_evaluator_1
        }
    };

    CalibrationData calibration_data_2 {
        .min_voltage = 0.0,
        .max_voltage = 3.3,
        .min_value = 0.0,
        .max_value = 200.0
    };

    ExpressionEvaluator expression_evaluator_2("x * 4 + {sensor_1} + 1.6");

    Sensor sensor_2 {
        .id = "sensor_2",
        .metadata = {
            .name = "Sensor 2",
            .unit = "km/h",
            .description = "Test Sensor 2"
        },
        .configuration = {
            .type = SensorType::PHYSICAL_ANALOG,
            .channel = 1,
            .calibration = calibration_data_2,
            .expression_evaluator = expression_evaluator_2
        }
    };

    UpdateSensors({sensor_1, sensor_2});
}

void SensorsConfigurationService::UpdateSensors(const std::vector<Sensor>& sensors) {
    SensorsOrderResolver resolver;

    for(const auto& sensor : sensors)
        resolver.AddSensor(sensor);

    ordered_sensors_.clear();
    ordered_sensors_ = resolver.GetProcessingOrder();
}

const std::vector<Sensor>& SensorsConfigurationService::GetSensors() const {
    return ordered_sensors_;
}

} // namespace eerie_leap::domain::sensor_domain::services