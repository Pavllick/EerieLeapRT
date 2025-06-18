#include <memory>
#include <zephyr/kernel.h>
#include <zephyr/ztest.h>

#include "configuration/system_config/system_config.h"
#include "configuration/services/configuration_service.h"
#include "controllers/system_configuration_controller.h"
#include "domain/fs_domain/services/fs_service.h"
#include "controllers/sensors_configuration_controller.h"

using namespace eerie_leap::configuration::services;
using namespace eerie_leap::domain::fs_domain::services;
using namespace eerie_leap::controllers;

ZTEST_SUITE(system_configuration_controller, NULL, NULL, NULL, NULL, NULL);

ZTEST(system_configuration_controller, test_SystemConfigurationController_Save_config_successfully_saved) {
    auto fs_service = std::make_shared<FsService>();
    fs_service->Format();

    auto system_configuration_service = std::make_shared<ConfigurationService<SystemConfig>>("system_config", fs_service);
    auto system_configuration_controller = std::make_shared<SystemConfigurationController>(system_configuration_service);

    SystemConfiguration system_configuration {
        .hw_version = 23456,
        .sw_version = 87654
    };
    auto system_configuration_ptr = std::make_shared<SystemConfiguration>(system_configuration);

    bool result = system_configuration_controller->Update(system_configuration_ptr);
    zassert_true(result);

    auto saved_system_configuration = *system_configuration_controller->Get();

    zassert_equal(saved_system_configuration.hw_version, system_configuration_ptr->hw_version);
    zassert_equal(saved_system_configuration.sw_version, system_configuration_ptr->sw_version);
}

ZTEST(system_configuration_controller, test_SystemConfigurationController_Save_config_and_Load) {
    auto fs_service = std::make_shared<FsService>();
    fs_service->Format();

    auto system_configuration_service = std::make_shared<ConfigurationService<SystemConfig>>("system_config", fs_service);
    auto system_configuration_controller = std::make_shared<SystemConfigurationController>(system_configuration_service);

    SystemConfiguration system_configuration {
        .hw_version = 34567,
        .sw_version = 98765
    };
    auto system_configuration_ptr = std::make_shared<SystemConfiguration>(system_configuration);

    bool result = system_configuration_controller->Update(system_configuration_ptr);
    zassert_true(result);

    system_configuration_controller = nullptr;
    system_configuration_controller = std::make_shared<SystemConfigurationController>(system_configuration_service);

    auto saved_system_configuration = *system_configuration_controller->Get();

    zassert_equal(saved_system_configuration.hw_version, system_configuration_ptr->hw_version);
    zassert_equal(saved_system_configuration.sw_version, system_configuration_ptr->sw_version);
}
