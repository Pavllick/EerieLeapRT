#include <memory>
#include <zephyr/kernel.h>
#include <zephyr/ztest.h>

#include "configuration/cbor/cbor_system_config/cbor_system_config.h"
#include "configuration/services/cbor_configuration_service.h"
#include "domain/system_domain/configuration/system_configuration_manager.h"

#include "subsys/device_tree/dt_fs.h"
#include "subsys/fs/services/fs_service.h"

using namespace eerie_leap::configuration::services;
using namespace eerie_leap::subsys::device_tree;
using namespace eerie_leap::subsys::fs::services;
using namespace eerie_leap::domain::system_domain::models;
using namespace eerie_leap::domain::system_domain::configuration;

ZTEST_SUITE(system_configuration_manager, NULL, NULL, NULL, NULL, NULL);

ZTEST(system_configuration_manager, test_SystemConfigurationManager_Save_config_successfully_saved) {
    DtFs::InitInternalFs();
    auto fs_service = std::make_shared<FsService>(DtFs::GetInternalFsMp().value());

    fs_service->Format();

    auto cbor_system_configuration_service = std::make_unique<CborConfigurationService<CborSystemConfig>>("system_config", fs_service);
    auto json_system_configuration_service = std::make_unique<JsonConfigurationService<JsonSystemConfig>>("system_config", fs_service);
    auto system_configuration_manager = std::make_shared<SystemConfigurationManager>(
        std::move(cbor_system_configuration_service), std::move(json_system_configuration_service));

    SystemConfiguration system_configuration(std::allocator_arg, Mrm::GetDefaultPmr());
    system_configuration.hw_version = 23456;
    system_configuration.sw_version = 87654;

    bool result = system_configuration_manager->Update(system_configuration);
    zassert_true(result);

    auto saved_system_configuration = system_configuration_manager->Get();

    zassert_equal(saved_system_configuration->hw_version, system_configuration.hw_version);
    zassert_equal(saved_system_configuration->sw_version, system_configuration.sw_version);
}

ZTEST(system_configuration_manager, test_SystemConfigurationManager_Save_config_and_Load) {
    DtFs::InitInternalFs();
    auto fs_service = std::make_shared<FsService>(DtFs::GetInternalFsMp().value());

    fs_service->Format();

    auto cbor_system_configuration_service = std::make_unique<CborConfigurationService<CborSystemConfig>>("system_config", fs_service);
    auto json_system_configuration_service = std::make_unique<JsonConfigurationService<JsonSystemConfig>>("system_config", fs_service);
    auto system_configuration_manager = std::make_shared<SystemConfigurationManager>(
        std::move(cbor_system_configuration_service), std::move(json_system_configuration_service));

    SystemConfiguration system_configuration(std::allocator_arg, Mrm::GetDefaultPmr());
    system_configuration.device_id = 14;

    bool result = system_configuration_manager->Update(system_configuration);
    zassert_true(result);

    cbor_system_configuration_service = std::make_unique<CborConfigurationService<CborSystemConfig>>("system_config", fs_service);
    json_system_configuration_service = std::make_unique<JsonConfigurationService<JsonSystemConfig>>("system_config", fs_service);
    system_configuration_manager = nullptr;
    system_configuration_manager = std::make_shared<SystemConfigurationManager>(
        std::move(cbor_system_configuration_service), std::move(json_system_configuration_service));

    auto saved_system_configuration = system_configuration_manager->Get();

    zassert_equal(saved_system_configuration->device_id, system_configuration.device_id);
    zassert_equal(saved_system_configuration->hw_version, 0x02090006);
    zassert_equal(saved_system_configuration->sw_version, 0x07080004);
}
