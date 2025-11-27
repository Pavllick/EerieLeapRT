#include <zephyr/ztest.h>

#include "domain/sensor_domain/models/sensor.h"
#include "utilities/voltage_interpolator/linear_voltage_interpolator.hpp"
#include "utilities/math_parser/expression_evaluator.h"
#include "domain/sensor_domain/utilities/sensors_order_resolver.h"

using namespace eerie_leap::utilities::math_parser;
using namespace eerie_leap::domain::sensor_domain::models;
using namespace eerie_leap::domain::sensor_domain::utilities;
using namespace eerie_leap::utilities::voltage_interpolator;

ZTEST_SUITE(sensors_order_resolver, NULL, NULL, NULL, NULL, NULL);

std::vector<std::shared_ptr<Sensor>> sensors_order_resolver_GetTestSensors() {
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
        .voltage_interpolator = std::make_unique<LinearVoltageInterpolator>(calibration_data_2_ptr),
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
        .channel = 4,
        .sampling_rate_ms = 2000
    };

    auto sensor_5 = std::make_shared<Sensor>("sensor_5");
    sensor_5->metadata = {
        .name = "Sensor 5",
        .unit = "km/h",
        .description = "Test Sensor 5"
    };
    sensor_5->configuration = {
        .type = SensorType::PHYSICAL_ANALOG,
        .channel = 4,
        .sampling_rate_ms = 2000,
        .expression_evaluator = std::make_unique<ExpressionEvaluator>("sensor_6 + 2.34")
    };

    auto sensor_6 = std::make_shared<Sensor>("sensor_6");
    sensor_6->metadata = {
        .name = "Sensor 6",
        .unit = "km/h",
        .description = "Test Sensor 6"
    };
    sensor_6->configuration = {
        .type = SensorType::PHYSICAL_ANALOG,
        .channel = 4,
        .sampling_rate_ms = 2000,
        .expression_evaluator = std::make_unique<ExpressionEvaluator>("sensor_5 + 4.34")
    };

    std::vector<std::shared_ptr<Sensor>> sensors = {
        sensor_1, sensor_2, sensor_3, sensor_4, sensor_5, sensor_6 };

    return sensors;
}

ZTEST(sensors_order_resolver, test_GetProcessingOrder) {
    auto sensors = sensors_order_resolver_GetTestSensors();

    auto sensors_order_resolver = std::make_shared<SensorsOrderResolver>();

    auto oredered_sensors = sensors_order_resolver->GetProcessingOrder();
    zassert_equal(oredered_sensors.size(), 0);

    sensors_order_resolver->AddSensor(sensors[1]);
    oredered_sensors = sensors_order_resolver->GetProcessingOrder();
    zassert_equal(oredered_sensors.size(), 1);

    sensors_order_resolver->AddSensor(sensors[0]);
    sensors_order_resolver->AddSensor(sensors[2]);
    oredered_sensors = sensors_order_resolver->GetProcessingOrder();
    zassert_equal(oredered_sensors.size(), 3);
    zassert_str_equal(oredered_sensors[0]->id.c_str(), "sensor_2");
    zassert_str_equal(oredered_sensors[1]->id.c_str(), "sensor_1");
    zassert_str_equal(oredered_sensors[2]->id.c_str(), "sensor_3");

    sensors_order_resolver->AddSensor(sensors[3]);
    oredered_sensors = sensors_order_resolver->GetProcessingOrder();
    zassert_equal(oredered_sensors.size(), 4);
    zassert_str_equal(oredered_sensors[0]->id.c_str(), "sensor_4");
    zassert_str_equal(oredered_sensors[1]->id.c_str(), "sensor_2");
    zassert_str_equal(oredered_sensors[2]->id.c_str(), "sensor_1");
    zassert_str_equal(oredered_sensors[3]->id.c_str(), "sensor_3");
}

ZTEST_EXPECT_FAIL(sensors_order_resolver, test_GetProcessingOrder_missing_dependency);
ZTEST(sensors_order_resolver, test_GetProcessingOrder_missing_dependency) {
    auto sensors = sensors_order_resolver_GetTestSensors();

    auto sensors_order_resolver = std::make_shared<SensorsOrderResolver>();

    auto oredered_sensors = sensors_order_resolver->GetProcessingOrder();
    zassert_equal(oredered_sensors.size(), 0);

    sensors_order_resolver->AddSensor(sensors[0]);
    sensors_order_resolver->AddSensor(sensors[2]);

    try {
        sensors_order_resolver->GetProcessingOrder();
        zassert_true(true, "GetProcessingOrder expected to fail, but it didn't.");
    } catch(...) {
        zassert_true(false, "GetProcessingOrder failed as expected due to missing dependency.");
    }
}

ZTEST_EXPECT_FAIL(sensors_order_resolver, test_GetProcessingOrder_has_cyclic_dependency);
ZTEST(sensors_order_resolver, test_GetProcessingOrder_has_cyclic_dependency) {
    auto sensors = sensors_order_resolver_GetTestSensors();

    auto sensors_order_resolver = std::make_shared<SensorsOrderResolver>();

    auto oredered_sensors = sensors_order_resolver->GetProcessingOrder();
    zassert_equal(oredered_sensors.size(), 0);

    sensors_order_resolver->AddSensor(sensors[0]);
    sensors_order_resolver->AddSensor(sensors[1]);
    sensors_order_resolver->AddSensor(sensors[4]);
    sensors_order_resolver->AddSensor(sensors[5]);

    try {
        sensors_order_resolver->GetProcessingOrder();
        zassert_true(true, "GetProcessingOrder expected to fail, but it didn't.");
    } catch(...) {
        zassert_true(false, "GetProcessingOrder failed as expected due to cyclic dependency.");
    }
}
