#include <memory>
#include <vector>
#include <zephyr/kernel.h>
#include <zephyr/ztest.h>

#include "utilities/cbor/cbor_helpers.hpp"
#include "utilities/math_parser/expression_evaluator.h"
#include "utilities/math_parser/math_parser_service.hpp"
#include "configuration/cbor/cbor_system_config/cbor_system_config.h"
#include "configuration/cbor/cbor_sensors_config/cbor_sensors_config.h"
#include "configuration/services/cbor_configuration_service.h"

#include "domain/sensor_domain/configuration/sensors_configuration_manager.h"

#include "subsys/device_tree/dt_fs.h"
#include "subsys/fs/services/i_fs_service.h"
#include "subsys/fs/services/fs_service.h"

#include "utilities/voltage_interpolator/linear_voltage_interpolator.hpp"
#include "utilities/voltage_interpolator/cubic_spline_voltage_interpolator.hpp"

using namespace eerie_leap::utilities::cbor;
using namespace eerie_leap::utilities::math_parser;
using namespace eerie_leap::configuration::services;
using namespace eerie_leap::subsys::fs::services;
using namespace eerie_leap::domain::sensor_domain::configuration;
using namespace eerie_leap::domain::sensor_domain::models;
using namespace eerie_leap::utilities::voltage_interpolator;
using namespace eerie_leap::subsys::device_tree;

ZTEST_SUITE(sensors_configuration_manager, NULL, NULL, NULL, NULL, NULL);

std::vector<std::shared_ptr<Sensor>> sensors_configuration_manager_SetupTestSensors(std::shared_ptr<MathParserService> math_parser_service) {
    std::vector<CalibrationData> calibration_data_1 {
        {0.0, 0.0},
        {3.3, 100.0}
    };
    auto calibration_data_1_ptr = std::make_shared<std::vector<CalibrationData>>(calibration_data_1);

    ExpressionEvaluator expression_evaluator_1(math_parser_service, "{x} * 2 + {sensor_2} + 1");

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
        .voltage_interpolator = make_unique_ext<LinearVoltageInterpolator>(calibration_data_1_ptr),
        .expression_evaluator = make_unique_ext<ExpressionEvaluator>(expression_evaluator_1)
    };

    std::vector<CalibrationData> calibration_data_2 {
        {0.0, 0.0},
        {1.0, 29.0},
        {2.0, 111.0},
        {2.5, 162.0},
        {3.3, 200.0}
    };
    auto calibration_data_2_ptr = std::make_shared<std::vector<CalibrationData>>(calibration_data_2);

    ExpressionEvaluator expression_evaluator_2(math_parser_service, "x * 4 + 1.6");

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
        .voltage_interpolator = make_unique_ext<CubicSplineVoltageInterpolator>(calibration_data_2_ptr),
        .expression_evaluator = make_unique_ext<ExpressionEvaluator>(expression_evaluator_2)
    };

    ExpressionEvaluator expression_evaluator_3(math_parser_service, "{sensor_1} + 8.34");

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
        .expression_evaluator = make_unique_ext<ExpressionEvaluator>(expression_evaluator_3)
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
        .voltage_interpolator = make_unique_ext<CubicSplineVoltageInterpolator>(calibration_data_2_ptr)
    };

    std::vector<std::shared_ptr<Sensor>> sensors = {
        sensor_1, sensor_2, sensor_3, sensor_4 };

    return sensors;
}

ZTEST(sensors_configuration_manager, test_SensorsConfigurationManager_Save_config_successfully_saved) {
    DtFs::InitInternalFs();
    auto fs_service = std::make_shared<FsService>(DtFs::GetInternalFsMp().value());

    fs_service->Format();

    auto cbor_sensors_configuration_service = make_unique_ext<CborConfigurationService<CborSensorsConfig>>("sensors_config", fs_service);
    auto json_sensors_configuration_service = make_unique_ext<JsonConfigurationService<JsonSensorsConfig>>("sensors_config", fs_service);

    auto math_parser_service = std::make_shared<MathParserService>();
    auto sensors_configuration_manager = std::make_shared<SensorsConfigurationManager>(
        math_parser_service,
        nullptr,
        std::move(cbor_sensors_configuration_service),
        std::move(json_sensors_configuration_service),
        16,
        16);

    auto sensors = sensors_configuration_manager_SetupTestSensors(math_parser_service);
    sensors_configuration_manager->Update(sensors);

    auto saved_sensors = *sensors_configuration_manager->Get();

    zassert_equal(saved_sensors.size(), sensors.size());

    for(size_t i = 0; i < sensors.size(); ++i) {
        std::shared_ptr<Sensor> saved_sensor = nullptr;
        for(size_t j = 0; j < saved_sensors.size(); ++j) {
            if(strcmp(saved_sensors[j]->id.c_str(), sensors[i]->id.c_str()) == 0) {
                saved_sensor = saved_sensors[j];
                break;
            }
        }
        zassert_true(saved_sensor != nullptr);

        zassert_true(saved_sensor->metadata.name == sensors[i]->metadata.name);
        zassert_true(saved_sensor->metadata.unit == sensors[i]->metadata.unit);
        zassert_true(saved_sensor->metadata.description == sensors[i]->metadata.description);

        zassert_true(saved_sensor->configuration.type == sensors[i]->configuration.type);
        if(saved_sensor->configuration.channel.has_value() && sensors[i]->configuration.channel.has_value()) {
            zassert_true(saved_sensor->configuration.channel.value() == sensors[i]->configuration.channel.value());
        } else {
            zassert_true(!saved_sensor->configuration.channel.has_value() && !sensors[i]->configuration.channel.has_value());
        }
        zassert_true(saved_sensor->configuration.sampling_rate_ms == sensors[i]->configuration.sampling_rate_ms);
        if(saved_sensor->configuration.voltage_interpolator != nullptr || sensors[i]->configuration.voltage_interpolator != nullptr)
            zassert_true(saved_sensor->configuration.voltage_interpolator->GetInterpolationMethod() == sensors[i]->configuration.voltage_interpolator->GetInterpolationMethod());
        if(saved_sensor->configuration.expression_evaluator != nullptr || sensors[i]->configuration.expression_evaluator != nullptr)
            zassert_true(saved_sensor->configuration.expression_evaluator->GetExpression() == sensors[i]->configuration.expression_evaluator->GetExpression());
    }
}

ZTEST(sensors_configuration_manager, test_SensorsConfigurationManager_Save_config_and_Load) {
    DtFs::InitInternalFs();
    auto fs_service = std::make_shared<FsService>(DtFs::GetInternalFsMp().value());

    fs_service->Format();

    auto cbor_sensors_configuration_service = make_unique_ext<CborConfigurationService<CborSensorsConfig>>("sensors_config", fs_service);
    auto json_sensors_configuration_service = make_unique_ext<JsonConfigurationService<JsonSensorsConfig>>("sensors_config", fs_service);

    auto math_parser_service = std::make_shared<MathParserService>();
    auto sensors_configuration_manager = std::make_shared<SensorsConfigurationManager>(
        math_parser_service,
        nullptr,
        std::move(cbor_sensors_configuration_service),
        std::move(json_sensors_configuration_service),
        16,
        16);

    auto sensors = sensors_configuration_manager_SetupTestSensors(math_parser_service);
    sensors_configuration_manager->Update(sensors);

    cbor_sensors_configuration_service = make_unique_ext<CborConfigurationService<CborSensorsConfig>>("sensors_config", fs_service);
    json_sensors_configuration_service = make_unique_ext<JsonConfigurationService<JsonSensorsConfig>>("sensors_config", fs_service);
    sensors_configuration_manager = nullptr;
    sensors_configuration_manager = std::make_shared<SensorsConfigurationManager>(
        math_parser_service,
        nullptr,
        std::move(cbor_sensors_configuration_service),
        std::move(json_sensors_configuration_service),
        16,
        16);

    auto saved_sensors = *sensors_configuration_manager->Get();

    zassert_equal(saved_sensors.size(), sensors.size());

    for(size_t i = 0; i < sensors.size(); ++i) {
        std::shared_ptr<Sensor> saved_sensor = nullptr;
        for(size_t j = 0; j < saved_sensors.size(); ++j) {
            if(strcmp(saved_sensors[j]->id.c_str(), sensors[i]->id.c_str()) == 0) {
                saved_sensor = saved_sensors[j];
                break;
            }
        }
        zassert_true(saved_sensor != nullptr);

        zassert_true(saved_sensor->metadata.name == sensors[i]->metadata.name);
        zassert_true(saved_sensor->metadata.unit == sensors[i]->metadata.unit);
        zassert_true(saved_sensor->metadata.description == sensors[i]->metadata.description);

        zassert_true(saved_sensor->configuration.type == sensors[i]->configuration.type);
        if(saved_sensor->configuration.channel.has_value() && sensors[i]->configuration.channel.has_value()) {
            zassert_true(saved_sensor->configuration.channel.value() == sensors[i]->configuration.channel.value());
        } else {
            zassert_true(!saved_sensor->configuration.channel.has_value() && !sensors[i]->configuration.channel.has_value());
        }
        zassert_true(saved_sensor->configuration.sampling_rate_ms == sensors[i]->configuration.sampling_rate_ms);
        if(saved_sensor->configuration.voltage_interpolator != nullptr || sensors[i]->configuration.voltage_interpolator != nullptr)
            zassert_true(saved_sensor->configuration.voltage_interpolator->GetInterpolationMethod() == sensors[i]->configuration.voltage_interpolator->GetInterpolationMethod());
        if(saved_sensor->configuration.expression_evaluator != nullptr || sensors[i]->configuration.expression_evaluator != nullptr)
            zassert_true(saved_sensor->configuration.expression_evaluator->GetExpression() == sensors[i]->configuration.expression_evaluator->GetExpression());
    }
}

ZTEST(sensors_configuration_manager, test_SensorsConfigurationManager_Save_config_valid_sensor_id) {
    DtFs::InitInternalFs();
    auto fs_service = std::make_shared<FsService>(DtFs::GetInternalFsMp().value());

    fs_service->Format();

    auto cbor_sensors_configuration_service = make_unique_ext<CborConfigurationService<CborSensorsConfig>>("sensors_config", fs_service);
    auto json_sensors_configuration_service = make_unique_ext<JsonConfigurationService<JsonSensorsConfig>>("sensors_config", fs_service);

    auto math_parser_service = std::make_shared<MathParserService>();
    auto sensors_configuration_manager = std::make_shared<SensorsConfigurationManager>(
        math_parser_service,
        nullptr,
        std::move(cbor_sensors_configuration_service),
        std::move(json_sensors_configuration_service),
        16,
        16);

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
        .sampling_rate_ms = 100,
        .voltage_interpolator = make_unique_ext<CubicSplineVoltageInterpolator>(calibration_data_1_ptr)
    };

    auto sensor_2 = std::make_shared<Sensor>("_sensor_2");
    sensor_2->metadata = {
        .name = "Sensor 2",
        .unit = "km/h",
        .description = "Test Sensor 2"
    };
    sensor_2->configuration = {
        .type = SensorType::PHYSICAL_ANALOG,
        .channel = 0,
        .sampling_rate_ms = 100,
        .voltage_interpolator = make_unique_ext<CubicSplineVoltageInterpolator>(calibration_data_1_ptr)
    };

    auto sensor_3 = std::make_shared<Sensor>("_");
    sensor_3->metadata = {
        .name = "Sensor 3",
        .unit = "km/h",
        .description = "Test Sensor 3"
    };
    sensor_3->configuration = {
        .type = SensorType::PHYSICAL_ANALOG,
        .channel = 0,
        .sampling_rate_ms = 100,
        .voltage_interpolator = make_unique_ext<CubicSplineVoltageInterpolator>(calibration_data_1_ptr)
    };

    std::vector<std::shared_ptr<Sensor>> sensors {
        sensor_1, sensor_2, sensor_3 };

    for(auto sensor : sensors)
        sensors_configuration_manager->Update({sensor});
}

ZTEST(sensors_configuration_manager, test_SensorsConfigurationManager_Save_config_invalid_sensor_id) {
    DtFs::InitInternalFs();
    auto fs_service = std::make_shared<FsService>(DtFs::GetInternalFsMp().value());

    fs_service->Format();

    auto cbor_sensors_configuration_service = make_unique_ext<CborConfigurationService<CborSensorsConfig>>("sensors_config", fs_service);
    auto json_sensors_configuration_service = make_unique_ext<JsonConfigurationService<JsonSensorsConfig>>("sensors_config", fs_service);

    auto math_parser_service = std::make_shared<MathParserService>();
    auto sensors_configuration_manager = std::make_shared<SensorsConfigurationManager>(
        math_parser_service,
        nullptr,
        std::move(cbor_sensors_configuration_service),
        std::move(json_sensors_configuration_service),
        16,
        16);

    std::vector<CalibrationData> calibration_data_1 {
        {0.0, 0.0},
        {3.3, 100.0}
    };
    auto calibration_data_1_ptr = std::make_shared<std::vector<CalibrationData>>(calibration_data_1);

    auto sensor_1 = std::make_shared<Sensor>("1_sensor_1");
    sensor_1->metadata = {
        .name = "Sensor 1",
        .unit = "km/h",
        .description = "Test Sensor 1"
    };
    sensor_1->configuration = {
        .type = SensorType::PHYSICAL_ANALOG,
        .channel = 0,
        .sampling_rate_ms = 100,
        .voltage_interpolator = make_unique_ext<CubicSplineVoltageInterpolator>(calibration_data_1_ptr)
    };

    auto sensor_2 = std::make_shared<Sensor>("#sensor_2");
    sensor_2->metadata = {
        .name = "Sensor 2",
        .unit = "km/h",
        .description = "Test Sensor 2"
    };
    sensor_2->configuration = {
        .type = SensorType::PHYSICAL_ANALOG,
        .channel = 0,
        .sampling_rate_ms = 100,
        .voltage_interpolator = make_unique_ext<CubicSplineVoltageInterpolator>(calibration_data_1_ptr)
    };

    auto sensor_3 = std::make_shared<Sensor>("3");
    sensor_3->metadata = {
        .name = "Sensor 3",
        .unit = "km/h",
        .description = "Test Sensor 3"
    };
    sensor_3->configuration = {
        .type = SensorType::PHYSICAL_ANALOG,
        .channel = 0,
        .sampling_rate_ms = 100,
        .voltage_interpolator = make_unique_ext<CubicSplineVoltageInterpolator>(calibration_data_1_ptr)
    };

    std::vector<std::shared_ptr<Sensor>> sensors {
        sensor_1, sensor_2, sensor_3 };

    for(auto sensor : sensors)
        zassert_false(sensors_configuration_manager->Update({sensor}), "Invalid sensor configuration update expected to fail.");
}
