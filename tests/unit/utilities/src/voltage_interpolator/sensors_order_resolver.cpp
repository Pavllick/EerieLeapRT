#include <zephyr/ztest.h>

#include "utilities/voltage_interpolator/linear_voltage_interpolator.hpp"
#include "utilities/voltage_interpolator/cubic_spline_voltage_interpolator.hpp"
#include "subsys/math_parser/expression_evaluator.h"
#include "domain/sensor_domain/models/sensor.h"
#include "domain/sensor_domain/utilities/sensors_order_resolver.h"

using namespace eerie_leap::utilities::voltage_interpolator;
using namespace eerie_leap::subsys::math_parser;
using namespace eerie_leap::domain::sensor_domain::models;
using namespace eerie_leap::domain::sensor_domain::utilities;

ZTEST_SUITE(sensors_order_resolver, NULL, NULL, NULL, NULL, NULL);

std::vector<std::shared_ptr<Sensor>> sensors_order_resolver_GetTestSensors() {
    std::pmr::vector<CalibrationData> calibration_data_1 {
        {0.0, 0.0},
        {3.3, 100.0}
    };
    auto calibration_data_1_ptr = std::make_shared<std::pmr::vector<CalibrationData>>(calibration_data_1);

    auto sensor_1 = std::make_shared<Sensor>(std::allocator_arg, Mrm::GetDefaultPmr(), "sensor_1");

    sensor_1->metadata.name = "Sensor 1";
    sensor_1->metadata.unit = "km/h";
    sensor_1->metadata.description = "Test Sensor 1";

    sensor_1->configuration.type = SensorType::PHYSICAL_ANALOG;
    sensor_1->configuration.channel = 0;
    sensor_1->configuration.sampling_rate_ms = 1000;
    sensor_1->configuration.voltage_interpolator = make_unique_pmr<LinearVoltageInterpolator>(Mrm::GetDefaultPmr(), calibration_data_1_ptr);
    sensor_1->configuration.expression_evaluator = make_unique_pmr<ExpressionEvaluator>(Mrm::GetDefaultPmr(), "x * 2 + sensor_2 + 1");

    std::pmr::vector<CalibrationData> calibration_data_2 {
        {0.0, 0.0},
        {1.0, 29.0},
        {2.0, 111.0},
        {2.5, 162.0},
        {3.3, 200.0}
    };
    auto calibration_data_2_ptr = std::make_shared<std::pmr::vector<CalibrationData>>(calibration_data_2);

    auto sensor_2 = std::make_shared<Sensor>(std::allocator_arg, Mrm::GetDefaultPmr(), "sensor_2");

    sensor_2->metadata.name = "Sensor 2";
    sensor_2->metadata.unit = "km/h";
    sensor_2->metadata.description = "Test Sensor 2";

    sensor_2->configuration.type = SensorType::PHYSICAL_ANALOG;
    sensor_2->configuration.channel = 1;
    sensor_2->configuration.sampling_rate_ms = 500;
    sensor_2->configuration.voltage_interpolator = make_unique_pmr<LinearVoltageInterpolator>(Mrm::GetDefaultPmr(), calibration_data_2_ptr);
    sensor_2->configuration.expression_evaluator = make_unique_pmr<ExpressionEvaluator>(Mrm::GetDefaultPmr(), "x * 4 + 1.6");

    auto sensor_3 = std::make_shared<Sensor>(std::allocator_arg, Mrm::GetDefaultPmr(), "sensor_3");

    sensor_3->metadata.name = "Sensor 3";
    sensor_3->metadata.unit = "km/h";
    sensor_3->metadata.description = "Test Sensor 3";

    sensor_3->configuration.type = SensorType::VIRTUAL_ANALOG;
    sensor_3->configuration.channel = std::nullopt;
    sensor_3->configuration.sampling_rate_ms = 2000;
    sensor_3->configuration.expression_evaluator = make_unique_pmr<ExpressionEvaluator>(Mrm::GetDefaultPmr(), "sensor_1 + 8.34");

    auto sensor_4 = std::make_shared<Sensor>(std::allocator_arg, Mrm::GetDefaultPmr(), "sensor_4");

    sensor_4->metadata.name = "Sensor 4";
    sensor_4->metadata.unit = "km/h";
    sensor_4->metadata.description = "Test Sensor 4";

    sensor_4->configuration.type = SensorType::PHYSICAL_ANALOG;
    sensor_4->configuration.channel = 4;
    sensor_4->configuration.sampling_rate_ms = 2000;
    sensor_4->configuration.voltage_interpolator = make_unique_pmr<CubicSplineVoltageInterpolator>(Mrm::GetDefaultPmr(), calibration_data_2_ptr);

    auto sensor_5 = std::make_shared<Sensor>(std::allocator_arg, Mrm::GetDefaultPmr(), "sensor_5");

    sensor_5->metadata.name = "Sensor 5";
    sensor_5->metadata.unit = "km/h";
    sensor_5->metadata.description = "Test Sensor 5";

    sensor_5->configuration.type = SensorType::PHYSICAL_ANALOG;
    sensor_5->configuration.channel = 4;
    sensor_5->configuration.sampling_rate_ms = 2000;
    sensor_5->configuration.expression_evaluator = make_unique_pmr<ExpressionEvaluator>(Mrm::GetDefaultPmr(), "sensor_6 + 2.34");

    auto sensor_6 = std::make_shared<Sensor>(std::allocator_arg, Mrm::GetDefaultPmr(), "sensor_6");
    sensor_6->metadata.name = "Sensor 6";
    sensor_6->metadata.unit = "km/h";
    sensor_6->metadata.description = "Test Sensor 6";

    sensor_6->configuration.type = SensorType::PHYSICAL_ANALOG;
    sensor_6->configuration.channel = 4;
    sensor_6->configuration.sampling_rate_ms = 2000;
    sensor_6->configuration.expression_evaluator = make_unique_pmr<ExpressionEvaluator>(Mrm::GetDefaultPmr(), "sensor_5 + 4.34");

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
