#include <memory>
#include <zephyr/ztest.h>

#include "utilities/cbor/cbor_helpers.hpp"
#include "configuration/cbor/cbor_system_config/cbor_system_config.h"
#include "configuration/cbor/cbor_sensors_config/cbor_sensors_config.h"
#include "configuration/services/cbor_configuration_service.h"

#include "subsys/device_tree/dt_fs.h"
#include "subsys/fs/services/i_fs_service.h"
#include "subsys/fs/services/fs_service.h"

using namespace eerie_leap::utilities::cbor;
using namespace eerie_leap::configuration::services;
using namespace eerie_leap::subsys::fs::services;
using namespace eerie_leap::subsys::device_tree;

ZTEST_SUITE(configuration_service, NULL, NULL, NULL, NULL, NULL);

ZTEST(configuration_service, test_CborSystemConfig_Save_config_successfully_saved) {
    CborSystemConfig system_config;
    memset(&system_config, 0, sizeof(system_config));

    system_config.device_id = 12;
    system_config.hw_version = 22;
    system_config.sw_version = 2422;

    DtFs::InitInternalFs();
    auto fs_service = std::make_shared<FsService>(DtFs::GetInternalFsMp().value());

    fs_service->Format();
    auto system_config_service = std::make_unique<CborConfigurationService<CborSystemConfig>>("system_config", fs_service);

    auto save_res = system_config_service->Save(&system_config);
    zassert_true(save_res);
}

ZTEST(configuration_service, test_CborSystemConfig_Load_config_successfully_saved_and_loaded) {
    CborSystemConfig system_config;
    memset(&system_config, 0, sizeof(system_config));

    system_config.device_id = 14;
    system_config.hw_version = 46;
    system_config.sw_version = 8624;

    DtFs::InitInternalFs();
    auto fs_service = std::make_shared<FsService>(DtFs::GetInternalFsMp().value());

    fs_service->Format();
    auto system_config_service = std::make_unique<CborConfigurationService<CborSystemConfig>>("system_config", fs_service);

    auto save_res = system_config_service->Save(&system_config);
    zassert_true(save_res);

    auto loaded_config = system_config_service->Load();
    zassert_true(loaded_config.has_value());
    zassert_equal(loaded_config.value().config->device_id, 14);
    zassert_equal(loaded_config.value().config->hw_version, 46);
    zassert_equal(loaded_config.value().config->sw_version, 8624);
}

ZTEST(configuration_service, test_CborSensorsConfig_Save_config_successfully_saved) {
    // Create first sensor config
    std::string sensor_1_name = "name 1";
    std::string sensor_1_unit = "km/h";
    std::string sensor_1_description = "";
    CborSensorMetadataConfig metadata_config_1 = {
        .name = CborHelpers::ToZcborString(&sensor_1_name),
        .unit = CborHelpers::ToZcborString(&sensor_1_unit),
        .description = CborHelpers::ToZcborString(&sensor_1_description)
    };

    std::string sensor_1_expression = "({var_d} + {y}) * 4";
    CborSensorConfigurationConfig configuration_config_1 = {
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
    CborSensorConfig sensor_config_1 = {
        .id = CborHelpers::ToZcborString(&sensor_1_id),
        .metadata = metadata_config_1,
        .configuration = configuration_config_1
    };

    // Create second sensor config
    std::string sensor_2_name = "name 2";
    std::string sensor_2_unit = "m/s";
    std::string sensor_2_description = "";
    CborSensorMetadataConfig metadata_config_2 = {
        .name = CborHelpers::ToZcborString(&sensor_2_name),
        .unit = CborHelpers::ToZcborString(&sensor_2_unit),
        .description = CborHelpers::ToZcborString(&sensor_2_description)
    };

    CborSensorCalibrationDataMap calibration_data_map_2 = {
        .float32float = {
            { 1.0, 2.0 },
            { 1.1, 2.1 },
            { 1.2, 2.2 },
            { 1.3, 2.3 }
        }
    };

    std::string sensor_2_expression = "({x} - 8 * {var_d}) / {f}";
    CborSensorConfigurationConfig configuration_config_2 = {
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
    CborSensorConfig sensor_config_2 = {
        .id = CborHelpers::ToZcborString(&sensor_2_id),
        .metadata = metadata_config_2,
        .configuration = configuration_config_2
    };

    CborSensorsConfig sensors_config;
    sensors_config.CborSensorConfig_m = { sensor_config_1, sensor_config_2 };

    DtFs::InitInternalFs();
    auto fs_service = std::make_shared<FsService>(DtFs::GetInternalFsMp().value());

    fs_service->Format();
    auto sensors_config_service = std::make_unique<CborConfigurationService<CborSensorsConfig>>("sensors_config", fs_service);

    auto save_res = sensors_config_service->Save(&sensors_config);
    zassert_true(save_res);
}

ZTEST(configuration_service, test_CborSensorsConfig_Load_config_successfully_saved_and_loaded) {
    // Create first sensor config
    std::string sensor_1_name = "name 3";
    std::string sensor_1_unit = "km/h";
    std::string sensor_1_description = "";
    CborSensorMetadataConfig metadata_config_1 = {
        .name = CborHelpers::ToZcborString(&sensor_1_name),
        .unit = CborHelpers::ToZcborString(&sensor_1_unit),
        .description = CborHelpers::ToZcborString(&sensor_1_description)
    };

    std::string sensor_1_expression = "({var_d} + {y}) * 4";
    CborSensorConfigurationConfig configuration_config_1 = {
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
    CborSensorConfig sensor_config_1 = {
        .id = CborHelpers::ToZcborString(&sensor_1_id),
        .metadata = metadata_config_1,
        .configuration = configuration_config_1
    };

    // Create second sensor config
    std::string sensor_2_name = "name 4";
    std::string sensor_2_unit = "m/s";
    std::string sensor_2_description = "Some description 4";
    CborSensorMetadataConfig metadata_config_2 = {
        .name = CborHelpers::ToZcborString(&sensor_2_name),
        .unit = CborHelpers::ToZcborString(&sensor_2_unit),
        .description = CborHelpers::ToZcborString(&sensor_2_description)
    };

    CborSensorCalibrationDataMap calibration_data_map_2 = {
        .float32float = {
            { 1.0, 2.0 },
            { 1.1, 2.1 },
            { 1.2, 2.2 },
            { 1.3, 2.3 }
        }
    };

    std::string sensor_2_expression = "({x} - 8 * {var_d}) / {f}";
    CborSensorConfigurationConfig configuration_config_2 = {
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
    CborSensorConfig sensor_config_2 = {
        .id = CborHelpers::ToZcborString(&sensor_2_id),
        .metadata = metadata_config_2,
        .configuration = configuration_config_2
    };

    // Create sensors config with both sensors
    CborSensorsConfig sensors_config = {
        .CborSensorConfig_m = { sensor_config_1, sensor_config_2 }
    };

    // Initialize services
    DtFs::InitInternalFs();
    auto fs_service = std::make_shared<FsService>(DtFs::GetInternalFsMp().value());

    fs_service->Format();
    auto sensors_config_service = std::make_unique<CborConfigurationService<CborSensorsConfig>>("sensors_config", fs_service);

    // Save config
    auto save_res = sensors_config_service->Save(&sensors_config);
    zassert_true(save_res);

    // Load config and verify
    auto loaded_config = sensors_config_service->Load();
    zassert_true(loaded_config.has_value());

    auto loaded_sensors_config = std::move(loaded_config.value().config);
    zassert_equal(loaded_sensors_config->CborSensorConfig_m.size(), 2);

    // Verify first sensor
    zassert_str_equal(CborHelpers::ToStdString(loaded_sensors_config->CborSensorConfig_m[0].id).c_str(), sensor_1_id.c_str());

    zassert_str_equal(CborHelpers::ToStdString(loaded_sensors_config->CborSensorConfig_m[0].metadata.name).c_str(), sensor_1_name.c_str());
    zassert_str_equal(CborHelpers::ToStdString(loaded_sensors_config->CborSensorConfig_m[0].metadata.unit).c_str(), sensor_1_unit.c_str());

    zassert_equal(loaded_sensors_config->CborSensorConfig_m[0].configuration.type, 1);
    zassert_equal(loaded_sensors_config->CborSensorConfig_m[0].configuration.channel, 6);
    zassert_true(loaded_sensors_config->CborSensorConfig_m[0].configuration.channel_present);
    zassert_equal(loaded_sensors_config->CborSensorConfig_m[0].configuration.sampling_rate_ms, 210);
    zassert_false(loaded_sensors_config->CborSensorConfig_m[0].configuration.calibration_table_present);
    zassert_equal(loaded_sensors_config->CborSensorConfig_m[0].configuration.interpolation_method, 1);
    zassert_true(loaded_sensors_config->CborSensorConfig_m[0].configuration.expression_present);
    zassert_str_equal(CborHelpers::ToStdString(loaded_sensors_config->CborSensorConfig_m[0].configuration.expression).c_str(), sensor_1_expression.c_str());

    // Verify second sensor
    zassert_str_equal(CborHelpers::ToStdString(loaded_sensors_config->CborSensorConfig_m[1].id).c_str(), sensor_2_id.c_str());

    zassert_str_equal(CborHelpers::ToStdString(loaded_sensors_config->CborSensorConfig_m[1].metadata.name).c_str(), sensor_2_name.c_str());
    zassert_str_equal(CborHelpers::ToStdString(loaded_sensors_config->CborSensorConfig_m[1].metadata.unit).c_str(), sensor_2_unit.c_str());
    zassert_str_equal(CborHelpers::ToStdString(loaded_sensors_config->CborSensorConfig_m[1].metadata.description).c_str(), sensor_2_description.c_str());

    zassert_equal(loaded_sensors_config->CborSensorConfig_m[1].configuration.type, 1);
    zassert_equal(loaded_sensors_config->CborSensorConfig_m[1].configuration.channel, 6);
    zassert_true(loaded_sensors_config->CborSensorConfig_m[1].configuration.channel_present);
    zassert_equal(loaded_sensors_config->CborSensorConfig_m[1].configuration.sampling_rate_ms, 250);
    zassert_true(loaded_sensors_config->CborSensorConfig_m[1].configuration.calibration_table_present);
    zassert_equal(loaded_sensors_config->CborSensorConfig_m[1].configuration.calibration_table.float32float.size(), 4);
    zassert_equal(loaded_sensors_config->CborSensorConfig_m[1].configuration.calibration_table.float32float[0].float32float_key, 1.0);
    zassert_equal(loaded_sensors_config->CborSensorConfig_m[1].configuration.calibration_table.float32float[0].float32float, 2.0);
    zassert_between_inclusive(loaded_sensors_config->CborSensorConfig_m[1].configuration.calibration_table.float32float[1].float32float_key, 1.09, 1.11);
    zassert_between_inclusive(loaded_sensors_config->CborSensorConfig_m[1].configuration.calibration_table.float32float[1].float32float, 2.09, 2.11);
    zassert_between_inclusive(loaded_sensors_config->CborSensorConfig_m[1].configuration.calibration_table.float32float[2].float32float_key, 1.19, 1.21);
    zassert_between_inclusive(loaded_sensors_config->CborSensorConfig_m[1].configuration.calibration_table.float32float[2].float32float, 2.19, 2.21);
    zassert_between_inclusive(loaded_sensors_config->CborSensorConfig_m[1].configuration.calibration_table.float32float[3].float32float_key, 1.29, 1.31);
    zassert_between_inclusive(loaded_sensors_config->CborSensorConfig_m[1].configuration.calibration_table.float32float[3].float32float, 2.29, 2.31);
    zassert_equal(loaded_sensors_config->CborSensorConfig_m[1].configuration.interpolation_method, 1);
    zassert_true(loaded_sensors_config->CborSensorConfig_m[1].configuration.expression_present);
    zassert_str_equal(CborHelpers::ToStdString(loaded_sensors_config->CborSensorConfig_m[1].configuration.expression).c_str(), sensor_2_expression.c_str());
}
