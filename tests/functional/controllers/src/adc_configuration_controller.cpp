#include <memory>
#include <zephyr/kernel.h>
#include <zephyr/ztest.h>

#include "configuration/adc_config/adc_config.h"
#include "configuration/services/configuration_service.h"
#include "subsys/fs/services/fs_service.h"
#include "controllers/adc_configuration_controller.h"

using namespace eerie_leap::configuration::services;
using namespace eerie_leap::subsys::fs::services;
using namespace eerie_leap::controllers;

ZTEST_SUITE(adc_configuration_controller, NULL, NULL, NULL, NULL, NULL);

#define PARTITION_NODE DT_ALIAS(lfs1)
FS_FSTAB_DECLARE_ENTRY(PARTITION_NODE);

std::shared_ptr<AdcConfiguration> adc_configuration_controller_GetTestConfiguration() {
    std::vector<CalibrationData> adc_calibration_data_samples {
        {0.0, 0.0},
        {5.0, 5.0}
    };

    auto adc_calibration_data_samples_ptr = make_shared_ext<std::vector<CalibrationData>>(adc_calibration_data_samples);
    auto adc_calibrator = make_shared_ext<AdcCalibrator>(InterpolationMethod::LINEAR, adc_calibration_data_samples_ptr);

    auto adc_channel_configuration = make_shared_ext<AdcChannelConfiguration>(adc_calibrator);

    std::vector<std::shared_ptr<AdcChannelConfiguration>> channel_configurations;
    channel_configurations.reserve(8);
    for(int i = 0; i < 8; i++)
        channel_configurations.push_back(adc_channel_configuration);

    auto adc_configuration = make_shared_ext<AdcConfiguration>();
    adc_configuration->samples = 40;
    adc_configuration->channel_configurations =
        make_shared_ext<std::vector<std::shared_ptr<AdcChannelConfiguration>>>(channel_configurations);

    return adc_configuration;
}

ZTEST(adc_configuration_controller, test_AdcConfigurationController_Save_config_successfully_saved) {
    auto fs_service = std::make_shared<FsService>(FS_FSTAB_ENTRY(PARTITION_NODE));
    fs_service->Format();

    auto adc_configuration_service = std::make_shared<ConfigurationService<AdcConfig>>("adc_config", fs_service);
    auto adc_configuration_controller = std::make_shared<AdcConfigurationController>(adc_configuration_service);

    auto adc_configuration = adc_configuration_controller_GetTestConfiguration();

    bool result = adc_configuration_controller->Update(adc_configuration);
    zassert_true(result);

    auto adc_manager = adc_configuration_controller->Get();

    for(int i = 0; i < adc_manager->GetChannelCount(); i++) {
        auto adc_channel_configuration = adc_manager->GetChannelConfiguration(i);

        zassert_true(adc_channel_configuration->calibrator != nullptr);
        zassert_equal(adc_channel_configuration->calibrator->GetInterpolationMethod(), InterpolationMethod::LINEAR);

        auto calibration_table = adc_channel_configuration->calibrator->GetCalibrationTable();
        zassert_equal(calibration_table->size(), 2);
        zassert_equal(calibration_table->at(0).voltage, 0.0);
        zassert_equal(calibration_table->at(0).value, 0.0);
        zassert_equal(calibration_table->at(1).voltage, 5.0);
        zassert_equal(calibration_table->at(1).value, 5.0);
    }
}

ZTEST(adc_configuration_controller, test_AdcConfigurationController_Save_config_and_Load) {
    auto fs_service = std::make_shared<FsService>(FS_FSTAB_ENTRY(PARTITION_NODE));
    fs_service->Format();

    auto adc_configuration_service = std::make_shared<ConfigurationService<AdcConfig>>("adc_config", fs_service);
    auto adc_configuration_controller = std::make_shared<AdcConfigurationController>(adc_configuration_service);

    auto adc_configuration = adc_configuration_controller_GetTestConfiguration();

    bool result = adc_configuration_controller->Update(adc_configuration);
    zassert_true(result);

    adc_configuration_controller = nullptr;
    adc_configuration_controller = std::make_shared<AdcConfigurationController>(adc_configuration_service);

    auto adc_manager = adc_configuration_controller->Get();

    for(int i = 0; i < adc_manager->GetChannelCount(); i++) {
        auto adc_channel_configuration = adc_manager->GetChannelConfiguration(i);

        zassert_true(adc_channel_configuration->calibrator != nullptr);
        zassert_equal(adc_channel_configuration->calibrator->GetInterpolationMethod(), InterpolationMethod::LINEAR);

        auto calibration_table = adc_channel_configuration->calibrator->GetCalibrationTable();
        zassert_equal(calibration_table->size(), 2);
        zassert_equal(calibration_table->at(0).voltage, 0.0);
        zassert_equal(calibration_table->at(0).value, 0.0);
        zassert_equal(calibration_table->at(1).voltage, 5.0);
        zassert_equal(calibration_table->at(1).value, 5.0);
    }
}
