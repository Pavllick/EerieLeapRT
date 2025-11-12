#include <zephyr/sys/crc.h>

#include "subsys/random/rng.h"

#include "system_configuration_manager.h"

namespace eerie_leap::domain::system_domain::configuration {

using namespace eerie_leap::subsys::random;

LOG_MODULE_REGISTER(system_config_mngr_logger);

SystemConfigurationManager::SystemConfigurationManager(ext_unique_ptr<CborConfigurationService<CborSystemConfig>> cbor_configuration_service)
    : cbor_configuration_service_(std::move(cbor_configuration_service)),
    config_(nullptr),
    configuration_(nullptr),
    sd_json_checksum_(0) {

    cbor_parser_ = std::make_unique<SystemConfigurationCborParser>();
    json_parser_ = std::make_unique<SystemConfigurationJsonParser>();
    std::shared_ptr<SystemConfiguration> configuration = nullptr;

    try {
        configuration = Get(true);
    } catch(const std::exception& e) {}

    if(configuration == nullptr) {
        if(!CreateDefaultConfiguration()) {
            LOG_ERR("Failed to create default System configuration.");
            return;
        }

        LOG_INF("Default System configuration loaded successfully.");

        configuration = Get();
    }

    UpdateHwVersion(configuration->hw_version);
    UpdateSwVersion(configuration->sw_version);

    configuration = Get();

    LOG_INF("System Configuration Manager initialized successfully.");

    LOG_INF("HW Version: %s, SW Version: %s",
        configuration->GetFormattedHwVersion().c_str(), configuration->GetFormattedSwVersion().c_str());
    LOG_INF("Device ID: %llu", configuration->device_id);
}

bool SystemConfigurationManager::UpdateBuildNumber(uint32_t build_number) {
    auto configuration = Get();
    if(configuration == nullptr)
        return false;

    if(build_number != configuration->build_number) {
        configuration->build_number = build_number;

        bool result = Update(configuration);
        if(!result)
            return false;

        LOG_INF("Build number updated to %u.", configuration->build_number);
    }

    return true;
}

bool SystemConfigurationManager::UpdateComUsers(const std::vector<ComUserConfiguration>& com_user_configurations) {
    auto configuration = Get();
    if(configuration == nullptr)
        return false;

    configuration->com_user_configurations = com_user_configurations;

    bool result = Update(configuration);
    if(!result)
        return false;

    LOG_INF("Com Users updated. Total count: %zu.", configuration->com_user_configurations.size());

    return true;
}

bool SystemConfigurationManager::UpdateHwVersion(uint32_t hw_version) {
    auto configuration = Get();
    if(configuration == nullptr)
        return false;

    uint32_t config_hw_version = SystemConfiguration::GetConfigHwVersion();
    if(hw_version != config_hw_version) {
        configuration->hw_version = config_hw_version;

        bool result = Update(configuration);
        if(!result)
            return false;

        LOG_INF("HW version updated to %s.", configuration->GetFormattedHwVersion().c_str());
    }

    return true;
}

bool SystemConfigurationManager::UpdateSwVersion(uint32_t sw_version) {
    auto configuration = Get();
    if(configuration == nullptr)
        return false;

    uint32_t config_sw_version = SystemConfiguration::GetConfigSwVersion();
    if(sw_version != config_sw_version) {
        configuration->sw_version = config_sw_version;
        configuration->build_number = 0;

        bool result = Update(configuration);
        if(!result)
            return false;

        LOG_INF("SW version updated to %s.", configuration->GetFormattedSwVersion().c_str());
    }

    return true;
}

bool SystemConfigurationManager::CreateDefaultConfiguration() {
    auto configuration = make_shared_ext<SystemConfiguration>();
    configuration->device_id = Rng::Get64(true);
    configuration->hw_version = 0;
    configuration->sw_version = 0;
    configuration->build_number = 0;

    return Update(configuration);
}

bool SystemConfigurationManager::Update(std::shared_ptr<SystemConfiguration> configuration) {
    auto config = cbor_parser_->Serialize(*configuration);
    config->sd_json_checksum = sd_json_checksum_;

    if(!cbor_configuration_service_->Save(config.get()))
        return false;

    Get(true);

    return true;
}

std::shared_ptr<SystemConfiguration> SystemConfigurationManager::Get(bool force_load) {
    if(configuration_ != nullptr && !force_load)
        return configuration_;

    auto config = cbor_configuration_service_->Load();
    if(!config.has_value())
        return nullptr;

    config_raw_ = std::move(config.value().config_raw);
    config_ = std::move(config.value().config);

    auto configuration = cbor_parser_->Deserialize(*config_);
    configuration_ = std::make_shared<SystemConfiguration>(configuration);

    sd_json_checksum_ = config_->sd_json_checksum;

    return configuration_;
}

} // namespace eerie_leap::domain::system_domain::configuration
