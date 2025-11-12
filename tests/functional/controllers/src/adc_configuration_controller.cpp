#include <memory>
#include <zephyr/kernel.h>
#include <zephyr/ztest.h>

#include "configuration/adc_config/adc_config.h"
#include "configuration/services/cbor_configuration_service.h"

#include "subsys/device_tree/dt_fs.h"
#include "subsys/fs/services/fs_service.h"

#include "domain/sensor_domain/configuration/adc_configuration_manager.h"

using namespace eerie_leap::configuration::services;
using namespace eerie_leap::subsys::fs::services;
using namespace eerie_leap::subsys::device_tree;
using namespace eerie_leap::domain::sensor_domain::configuration;

ZTEST_SUITE(adc_configuration_manager, NULL, NULL, NULL, NULL, NULL);

AdcConfiguration adc_configuration_manager_GetTestConfiguration() {
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

    AdcConfiguration adc_configuration;
    adc_configuration.samples = 40;
    adc_configuration.channel_configurations =
        make_shared_ext<std::vector<std::shared_ptr<AdcChannelConfiguration>>>(channel_configurations);

    return adc_configuration;
}

ZTEST(adc_configuration_manager, test_AdcConfigurationManager_Save_config_successfully_saved) {
    DtFs::InitInternalFs();
    auto fs_service = std::make_shared<FsService>(DtFs::GetInternalFsMp().value());

    fs_service->Format();

    auto adc_configuration_service = make_unique_ext<CborConfigurationService<AdcConfig>>("adc_config", fs_service);
    auto adc_configuration_manager = std::make_shared<AdcConfigurationManager>(std::move(adc_configuration_service));

    auto adc_configuration = adc_configuration_manager_GetTestConfiguration();

    bool result = adc_configuration_manager->Update(adc_configuration);
    zassert_true(result);

    auto adc_manager = adc_configuration_manager->Get();

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

ZTEST(adc_configuration_manager, test_AdcConfigurationManager_Save_config_and_Load) {
    DtFs::InitInternalFs();
    auto fs_service = std::make_shared<FsService>(DtFs::GetInternalFsMp().value());

    fs_service->Format();

    auto adc_configuration_service = make_unique_ext<CborConfigurationService<AdcConfig>>("adc_config", fs_service);
    auto adc_configuration_manager = std::make_shared<AdcConfigurationManager>(std::move(adc_configuration_service));

    auto adc_configuration = adc_configuration_manager_GetTestConfiguration();

    bool result = adc_configuration_manager->Update(adc_configuration);
    zassert_true(result);

    adc_configuration_service = make_unique_ext<CborConfigurationService<AdcConfig>>("adc_config", fs_service);
    adc_configuration_manager = nullptr;
    adc_configuration_manager = std::make_shared<AdcConfigurationManager>(std::move(adc_configuration_service));

    auto adc_manager = adc_configuration_manager->Get();

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
