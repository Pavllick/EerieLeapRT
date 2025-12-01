#include <zephyr/ztest.h>

#include "utilities/voltage_interpolator/linear_voltage_interpolator.hpp"
#include "utilities/voltage_interpolator/cubic_spline_voltage_interpolator.hpp"
#include "subsys/math_parser/expression_evaluator.h"

#include "domain/sensor_domain/configuration/parsers/sensors_cbor_parser.h"
#include "domain/sensor_domain/configuration/parsers/sensors_json_parser.h"
#include "domain/sensor_domain/models/sensor.h"

using namespace eerie_leap::subsys::math_parser;

using namespace eerie_leap::domain::sensor_domain::configuration::parsers;
using namespace eerie_leap::domain::sensor_domain::models;
using namespace eerie_leap::utilities::voltage_interpolator;

ZTEST_SUITE(sensors_parser, NULL, NULL, NULL, NULL, NULL);

std::vector<std::shared_ptr<Sensor>> sensors_parser_GetTestSensors() {
    std::vector<CalibrationData> calibration_data_1 {
        {0.0, 0.0},
        {3.3, 100.0}
    };
    auto calibration_data_1_ptr = std::make_shared<std::vector<CalibrationData>>(calibration_data_1);

    auto sensor_1 = std::make_shared<Sensor>("sensor_1");
    sensor_1->metadata = {
        .name = "Sensor 1",
        .unit = "km/h",
        .description = "Test Sensor 1"
    };
    sensor_1->configuration = {
        .type = SensorType::PHYSICAL_ANALOG,
        .channel = 0,
        .sampling_rate_ms = 1000,
        .voltage_interpolator = std::make_unique<LinearVoltageInterpolator>(calibration_data_1_ptr),
        .expression_evaluator = std::make_unique<ExpressionEvaluator>("x * 2 + sensor_2 + 1")
    };

    std::vector<CalibrationData> calibration_data_2 {
        {0.0, 0.0},
        {1.0, 29.0},
        {2.0, 111.0},
        {2.5, 162.0},
        {3.3, 200.0}
    };
    auto calibration_data_2_ptr = std::make_shared<std::vector<CalibrationData>>(calibration_data_2);

    auto sensor_2 = std::make_shared<Sensor>("sensor_2");
    sensor_2->metadata = {
        .name = "Sensor 2",
        .unit = "km/h",
        .description = "Test Sensor 2"
    };
    sensor_2->configuration = {
        .type = SensorType::PHYSICAL_ANALOG,
        .channel = 1,
        .sampling_rate_ms = 500,
        .voltage_interpolator = std::make_unique<CubicSplineVoltageInterpolator>(calibration_data_2_ptr),
        .expression_evaluator = std::make_unique<ExpressionEvaluator>("x * 4 + 1.6")
    };

    auto sensor_3 = std::make_shared<Sensor>("sensor_3");
    sensor_3->metadata = {
        .name = "Sensor 3",
        .unit = "km/h",
        .description = "Test Sensor 3"
    };
    sensor_3->configuration = {
        .type = SensorType::VIRTUAL_ANALOG,
        .channel = std::nullopt,
        .sampling_rate_ms = 2000,
        .expression_evaluator = std::make_unique<ExpressionEvaluator>("sensor_1 + 8.34")
    };

    auto sensor_4 = std::make_shared<Sensor>("sensor_4");
    sensor_4->metadata = {
        .name = "Sensor 4",
        .unit = "km/h",
        .description = "Test Sensor 4"
    };
    sensor_4->configuration = {
        .type = SensorType::PHYSICAL_ANALOG,
        .channel = 2,
        .sampling_rate_ms = 2000,
        .voltage_interpolator = std::make_unique<CubicSplineVoltageInterpolator>(calibration_data_2_ptr)
    };

    std::vector<std::shared_ptr<Sensor>> sensors = {
        sensor_1, sensor_2, sensor_3, sensor_4 };

    return sensors;
}

void sensors_parser_CompareSensors(
    std::vector<std::shared_ptr<Sensor>>& sensors,
    std::vector<std::shared_ptr<Sensor>>& deserialized_sensors) {

    zassert_equal(deserialized_sensors.size(), sensors.size());

    for(size_t i = 0; i < sensors.size(); ++i) {
        std::shared_ptr<Sensor> deserialized_sensor = nullptr;
        for(size_t j = 0; j < deserialized_sensors.size(); ++j) {
            if(strcmp(deserialized_sensors[j]->id.c_str(), sensors[i]->id.c_str()) == 0) {
                deserialized_sensor = deserialized_sensors[j];
                break;
            }
        }
        zassert_true(deserialized_sensor != nullptr);

        zassert_true(deserialized_sensor->metadata.name == sensors[i]->metadata.name);
        zassert_true(deserialized_sensor->metadata.unit == sensors[i]->metadata.unit);
        zassert_true(deserialized_sensor->metadata.description == sensors[i]->metadata.description);

        zassert_true(deserialized_sensor->configuration.type == sensors[i]->configuration.type);
        if(deserialized_sensor->configuration.channel.has_value() && sensors[i]->configuration.channel.has_value()) {
            zassert_true(deserialized_sensor->configuration.channel.value() == sensors[i]->configuration.channel.value());
        } else {
            zassert_true(!deserialized_sensor->configuration.channel.has_value() && !sensors[i]->configuration.channel.has_value());
        }
        zassert_true(deserialized_sensor->configuration.sampling_rate_ms == sensors[i]->configuration.sampling_rate_ms);
        if(deserialized_sensor->configuration.voltage_interpolator != nullptr || sensors[i]->configuration.voltage_interpolator != nullptr)
            zassert_true(deserialized_sensor->configuration.voltage_interpolator->GetInterpolationMethod() == sensors[i]->configuration.voltage_interpolator->GetInterpolationMethod());
        if(deserialized_sensor->configuration.expression_evaluator != nullptr || sensors[i]->configuration.expression_evaluator != nullptr)
            zassert_true(deserialized_sensor->configuration.expression_evaluator->GetExpression() == sensors[i]->configuration.expression_evaluator->GetExpression());
    }
}

ZTEST(sensors_parser, test_CborSerializeDeserialize) {
    auto sensors_cbor_parser = std::make_shared<SensorsCborParser>(nullptr);

    auto sensors = sensors_parser_GetTestSensors();

    auto serialized_sensors = sensors_cbor_parser->Serialize(sensors, 16, 16);
    auto deserialized_sensors = sensors_cbor_parser->Deserialize(*serialized_sensors.get(), 16, 16);

    sensors_parser_CompareSensors(sensors, deserialized_sensors);
}

ZTEST(sensors_parser, test_JsonSerializeDeserialize) {
    auto sensors_json_parser = std::make_shared<SensorsJsonParser>(nullptr);

    auto sensors = sensors_parser_GetTestSensors();

    auto serialized_sensors = sensors_json_parser->Serialize(sensors, 16, 16);
    auto deserialized_sensors = sensors_json_parser->Deserialize(*serialized_sensors.get(), 16, 16);

    sensors_parser_CompareSensors(sensors, deserialized_sensors);
}
