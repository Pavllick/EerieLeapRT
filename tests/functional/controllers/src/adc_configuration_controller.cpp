#include <memory>
#include <zephyr/kernel.h>
#include <zephyr/ztest.h>

#include "configuration/adc_config/adc_config.h"
#include "configuration/services/configuration_service.hpp"
#include "controllers/adc_configuration_controller.h"
#include "domain/fs_domain/services/fs_service.h"
#include "controllers/adc_configuration_controller.h"

using namespace eerie_leap::configuration::services;
using namespace eerie_leap::domain::fs_domain::services;
using namespace eerie_leap::controllers;

ZTEST_SUITE(adc_configuration_controller, NULL, NULL, NULL, NULL, NULL);

ZTEST(adc_configuration_controller, test_AdcConfigurationController_Save_config_successfully_saved) {
    auto fs_service = std::make_shared<FsService>();
    fs_service->Format();

    auto adc_configuration_service = std::make_shared<ConfigurationService<AdcConfig>>("adc_config", fs_service);
    auto adc_configuration_controller = std::make_shared<AdcConfigurationController>(adc_configuration_service);

    AdcConfiguration adc_configuration {
        .name = "ESP32S3 Built-in ADC",
        .samples = 6
    };
    auto adc_configuration_ptr = std::make_shared<AdcConfiguration>(adc_configuration);

    bool result = adc_configuration_controller->Update(adc_configuration_ptr);
    zassert_true(result);

    auto saved_adc_configuration = *adc_configuration_controller->Get();

    zassert_equal(saved_adc_configuration.name, adc_configuration_ptr->name);
    zassert_equal(saved_adc_configuration.samples, adc_configuration_ptr->samples);
}

ZTEST(adc_configuration_controller, test_AdcConfigurationController_Save_config_and_Load) {
    auto fs_service = std::make_shared<FsService>();
    fs_service->Format();

    auto adc_configuration_service = std::make_shared<ConfigurationService<AdcConfig>>("adc_config", fs_service);
    auto adc_configuration_controller = std::make_shared<AdcConfigurationController>(adc_configuration_service);

    AdcConfiguration adc_configuration {
        .name = "Emulator ADC",
        .samples = 8
    };
    auto adc_configuration_ptr = std::make_shared<AdcConfiguration>(adc_configuration);

    bool result = adc_configuration_controller->Update(adc_configuration_ptr);
    zassert_true(result);

    adc_configuration_controller = nullptr;
    adc_configuration_controller = std::make_shared<AdcConfigurationController>(adc_configuration_service);

    auto saved_adc_configuration = *adc_configuration_controller->Get();

    zassert_equal(saved_adc_configuration.name, adc_configuration_ptr->name);
    zassert_equal(saved_adc_configuration.samples, adc_configuration_ptr->samples);
}
