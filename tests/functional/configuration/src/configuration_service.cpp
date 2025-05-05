#include <memory>
#include <zephyr/kernel.h>
#include <zephyr/ztest.h>

#include "utilities/cbor/cbor_helpers.hpp"
#include "configuration/system_config/system_config.h"
#include "configuration/sensor_config/sensor_config.h"
#include "configuration/services/configuration_service.hpp"
#include "domain/fs_domain/services/i_fs_service.h"
#include "domain/fs_domain/services/fs_service.h"

using namespace eerie_leap::utilities::cbor;
using namespace eerie_leap::configuration::services;
using namespace eerie_leap::domain::fs_domain::services;

ZTEST_SUITE(configuration_service, NULL, NULL, NULL, NULL, NULL);

ZTEST(configuration_service, test_SystemConfig_Save_config_successfully_saved) {
    SystemConfig system_config;
    system_config.hw_version = 22;
    system_config.sw_version = 2422;

    auto fs_service = std::make_shared<FsService>();
    auto system_config_service = std::make_shared<ConfigurationService<SystemConfig>>("system_config", fs_service);

    auto save_res = system_config_service->Save(system_config);
    zassert_true(save_res);
}

ZTEST(configuration_service, test_SystemConfig_Load_config_successfully_saved_and_loaded) {
    SystemConfig system_config;
    system_config.hw_version = 46;
    system_config.sw_version = 8624;

    auto fs_service = std::make_shared<FsService>();
    auto system_config_service = std::make_shared<ConfigurationService<SystemConfig>>("system_config", fs_service);

    auto save_res = system_config_service->Save(system_config);
    zassert_true(save_res);

    auto loaded_config = system_config_service->Load();
    zassert_true(loaded_config.has_value());
    zassert_equal(loaded_config.value().hw_version, 46);
    zassert_equal(loaded_config.value().sw_version, 8624);
}

ZTEST(configuration_service, test_SensorsConfig_Save_config_successfully_saved) {
    // Create first sensor config
    std::string sensor_1_name = "name 1";
    std::string sensor_1_unit = "km/h";
    SensorMetadataConfig metadata_config_1 = {
        .name = CborHelpers::ToZcborString(&sensor_1_name),
        .unit = CborHelpers::ToZcborString(&sensor_1_unit),
        .description_present = false
    };

    std::string sensor_1_expression = "({var_d} + {y}) * 4";
    SensorConfigurationConfig configuration_config_1 = {
        .type = 1,
        .sampling_rate_ms = 210,
        .interpolation_method = 1,
        .channel = 6,
        .channel_present = true,
        .calibration_table_present = false,
        .expression = CborHelpers::ToZcborString(&sensor_1_expression),
        .expression_present = true
    };

    std::string sensor_1_id = "sensor_1";
    SensorConfig sensor_config_1 = {
        .id = CborHelpers::ToZcborString(&sensor_1_id),
        .metadata = metadata_config_1,
        .configuration = configuration_config_1
    };

    // Create second sensor config
    std::string sensor_2_name = "name 2";
    std::string sensor_2_unit = "m/s";
    std::string sensor_2_description = "Some description 2";
    SensorMetadataConfig metadata_config_2 = {
        .name = CborHelpers::ToZcborString(&sensor_2_name),
        .unit = CborHelpers::ToZcborString(&sensor_2_unit),
        .description = CborHelpers::ToZcborString(&sensor_2_description),
        .description_present = true
    };

    SensorCalibrationDataMap calibration_data_map_2 = {
        .floatfloat = {
            { 1.0, 2.0 },
            { 1.1, 2.1 },
            { 1.2, 2.2 },
            { 1.3, 2.3 }
        },
        .floatfloat_count = 4
    };

    std::string sensor_2_expression = "({x} - 8 * {var_d}) / {f}";
    SensorConfigurationConfig configuration_config_2 = {
        .type = 1,
        .sampling_rate_ms = 250,
        .interpolation_method = 1,
        .channel = 6,
        .channel_present = true,
        .calibration_table = calibration_data_map_2,
        .calibration_table_present = true,
        .expression = CborHelpers::ToZcborString(&sensor_2_expression),
        .expression_present = true
    };

    std::string sensor_2_id = "sensor_2";
    SensorConfig sensor_config_2 = {
        .id = CborHelpers::ToZcborString(&sensor_2_id),
        .metadata = metadata_config_2,
        .configuration = configuration_config_2
    };

    SensorsConfig sensors_config = { sensor_config_1, sensor_config_2 };

    auto fs_service = std::make_shared<FsService>();
    auto sensors_config_service = std::make_shared<ConfigurationService<SensorsConfig>>("sensors_config", fs_service);

    auto save_res = sensors_config_service->Save(sensors_config);
    zassert_true(save_res);
}

ZTEST(configuration_service, test_SensorsConfig_Load_config_successfully_saved_and_loaded) {
    // Create first sensor config
    std::string sensor_1_name = "name 3";
    std::string sensor_1_unit = "km/h";
    SensorMetadataConfig metadata_config_1 = {
        .name = CborHelpers::ToZcborString(&sensor_1_name),
        .unit = CborHelpers::ToZcborString(&sensor_1_unit),
        .description_present = false
    };

    std::string sensor_1_expression = "({var_d} + {y}) * 4";
    SensorConfigurationConfig configuration_config_1 = {
        .type = 1,
        .sampling_rate_ms = 210,
        .interpolation_method = 1,
        .channel = 6,
        .channel_present = true,
        .calibration_table_present = false,
        .expression = CborHelpers::ToZcborString(&sensor_1_expression),
        .expression_present = true
    };

    std::string sensor_1_id = "sensor_3";
    SensorConfig sensor_config_1 = {
        .id = CborHelpers::ToZcborString(&sensor_1_id),
        .metadata = metadata_config_1,
        .configuration = configuration_config_1
    };

    // Create second sensor config
    std::string sensor_2_name = "name 4";
    std::string sensor_2_unit = "m/s";
    std::string sensor_2_description = "Some description 4";
    SensorMetadataConfig metadata_config_2 = {
        .name = CborHelpers::ToZcborString(&sensor_2_name),
        .unit = CborHelpers::ToZcborString(&sensor_2_unit),
        .description = CborHelpers::ToZcborString(&sensor_2_description),
        .description_present = true
    };

    SensorCalibrationDataMap calibration_data_map_2 = {
        .floatfloat = {
            { 1.0, 2.0 },
            { 1.1, 2.1 },
            { 1.2, 2.2 },
            { 1.3, 2.3 }
        },
        .floatfloat_count = 4
    };

    std::string sensor_2_expression = "({x} - 8 * {var_d}) / {f}";
    SensorConfigurationConfig configuration_config_2 = {
        .type = 1,
        .sampling_rate_ms = 250,
        .interpolation_method = 1,
        .channel = 6,
        .channel_present = true,
        .calibration_table = calibration_data_map_2,
        .calibration_table_present = true,
        .expression = CborHelpers::ToZcborString(&sensor_2_expression),
        .expression_present = true
    };

    std::string sensor_2_id = "sensor_4";
    SensorConfig sensor_config_2 = {
        .id = CborHelpers::ToZcborString(&sensor_2_id),
        .metadata = metadata_config_2,
        .configuration = configuration_config_2
    };

    // Create sensors config with both sensors
    SensorsConfig sensors_config = {
        .SensorConfig_m = { sensor_config_1, sensor_config_2 },
        .SensorConfig_m_count = 2
    };

    // Initialize services
    auto fs_service = std::make_shared<FsService>();
    auto sensors_config_service = std::make_shared<ConfigurationService<SensorsConfig>>("sensors_config", fs_service);

    // Save config
    auto save_res = sensors_config_service->Save(sensors_config);
    zassert_true(save_res);

    // Load config and verify
    auto loaded_config = sensors_config_service->Load();
    zassert_true(loaded_config.has_value());

    auto loaded_sensors_config = loaded_config.value();
    zassert_equal(loaded_sensors_config.SensorConfig_m_count, 2);

    // Verify first sensor
    zassert_str_equal(CborHelpers::ToStdString(loaded_sensors_config.SensorConfig_m[0].id).c_str(), sensor_1_id.c_str());

    zassert_str_equal(CborHelpers::ToStdString(loaded_sensors_config.SensorConfig_m[0].metadata.name).c_str(), sensor_1_name.c_str());
    zassert_str_equal(CborHelpers::ToStdString(loaded_sensors_config.SensorConfig_m[0].metadata.unit).c_str(), sensor_1_unit.c_str());
    zassert_false(loaded_sensors_config.SensorConfig_m[0].metadata.description_present);

    zassert_equal(loaded_sensors_config.SensorConfig_m[0].configuration.type, 1);
    zassert_equal(loaded_sensors_config.SensorConfig_m[0].configuration.channel, 6);
    zassert_true(loaded_sensors_config.SensorConfig_m[0].configuration.channel_present);
    zassert_equal(loaded_sensors_config.SensorConfig_m[0].configuration.sampling_rate_ms, 210);
    zassert_false(loaded_sensors_config.SensorConfig_m[0].configuration.calibration_table_present);
    zassert_equal(loaded_sensors_config.SensorConfig_m[0].configuration.interpolation_method, 1);
    zassert_true(loaded_sensors_config.SensorConfig_m[0].configuration.expression_present);
    zassert_str_equal(CborHelpers::ToStdString(loaded_sensors_config.SensorConfig_m[0].configuration.expression).c_str(), sensor_1_expression.c_str());

    // Verify second sensor
    zassert_str_equal(CborHelpers::ToStdString(loaded_sensors_config.SensorConfig_m[1].id).c_str(), sensor_2_id.c_str());

    zassert_str_equal(CborHelpers::ToStdString(loaded_sensors_config.SensorConfig_m[1].metadata.name).c_str(), sensor_2_name.c_str());
    zassert_str_equal(CborHelpers::ToStdString(loaded_sensors_config.SensorConfig_m[1].metadata.unit).c_str(), sensor_2_unit.c_str());
    zassert_true(loaded_sensors_config.SensorConfig_m[1].metadata.description_present);
    zassert_str_equal(CborHelpers::ToStdString(loaded_sensors_config.SensorConfig_m[1].metadata.description).c_str(), sensor_2_description.c_str());

    zassert_equal(loaded_sensors_config.SensorConfig_m[1].configuration.type, 1);
    zassert_equal(loaded_sensors_config.SensorConfig_m[1].configuration.channel, 6);
    zassert_true(loaded_sensors_config.SensorConfig_m[1].configuration.channel_present);
    zassert_equal(loaded_sensors_config.SensorConfig_m[1].configuration.sampling_rate_ms, 250);
    zassert_true(loaded_sensors_config.SensorConfig_m[1].configuration.calibration_table_present);
    zassert_equal(loaded_sensors_config.SensorConfig_m[1].configuration.calibration_table.floatfloat_count, 4);
    zassert_equal(loaded_sensors_config.SensorConfig_m[1].configuration.calibration_table.floatfloat[0].floatfloat_key, 1.0);
    zassert_equal(loaded_sensors_config.SensorConfig_m[1].configuration.calibration_table.floatfloat[0].floatfloat, 2.0);
    zassert_equal(loaded_sensors_config.SensorConfig_m[1].configuration.calibration_table.floatfloat[1].floatfloat_key, 1.1);
    zassert_equal(loaded_sensors_config.SensorConfig_m[1].configuration.calibration_table.floatfloat[1].floatfloat, 2.1);
    zassert_equal(loaded_sensors_config.SensorConfig_m[1].configuration.calibration_table.floatfloat[2].floatfloat_key, 1.2);
    zassert_equal(loaded_sensors_config.SensorConfig_m[1].configuration.calibration_table.floatfloat[2].floatfloat, 2.2);
    zassert_equal(loaded_sensors_config.SensorConfig_m[1].configuration.calibration_table.floatfloat[3].floatfloat_key, 1.3);
    zassert_equal(loaded_sensors_config.SensorConfig_m[1].configuration.calibration_table.floatfloat[3].floatfloat, 2.3);
    zassert_equal(loaded_sensors_config.SensorConfig_m[1].configuration.interpolation_method, 1);
    zassert_true(loaded_sensors_config.SensorConfig_m[1].configuration.expression_present);
    zassert_str_equal(CborHelpers::ToStdString(loaded_sensors_config.SensorConfig_m[1].configuration.expression).c_str(), sensor_2_expression.c_str());
}