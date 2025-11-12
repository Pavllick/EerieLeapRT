#include <zephyr/sys/crc.h>

#include "subsys/random/rng.h"

#include "system_configuration_manager.h"

namespace eerie_leap::domain::system_domain::configuration {

using namespace eerie_leap::subsys::random;

LOG_MODULE_REGISTER(system_config_mngr_logger);

SystemConfigurationManager::SystemConfigurationManager(
    ext_unique_ptr<CborConfigurationService<CborSystemConfig>> cbor_configuration_service,
    ext_unique_ptr<JsonConfigurationService<JsonSystemConfig>> json_configuration_service)
        : cbor_configuration_service_(std::move(cbor_configuration_service)),
        json_configuration_service_(std::move(json_configuration_service)),
        cbor_config_(nullptr),
        configuration_(nullptr),
        json_config_checksum_(0) {

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

    ApplyJsonConfiguration();

    configuration = Get();

    LOG_INF("System Configuration Manager initialized successfully.");

    LOG_INF("HW Version: %s, SW Version: %s",
        configuration->GetFormattedHwVersion().c_str(), configuration->GetFormattedSwVersion().c_str());
    LOG_INF("Device ID: %llu", configuration->device_id);
}

bool SystemConfigurationManager::ApplyJsonConfiguration() {
    if(!json_configuration_service_->IsAvailable())
        return false;

    auto json_config_loaded = json_configuration_service_->Load();
    if(json_config_loaded.has_value()) {
        uint32_t crc = crc32_ieee(json_config_loaded->config_raw->data(), json_config_loaded->config_raw->size());
        if(crc == json_config_checksum_)
            return true;

        auto configuration = json_parser_->Deserialize(
            *json_config_loaded->config,
            configuration_->device_id,
            configuration_->hw_version,
            configuration_->sw_version,
            configuration_->build_number);

        bool result = Update(std::make_shared<SystemConfiguration>(configuration));
        if(!result)
            return false;

        LOG_INF("JSON configuration loaded successfully.");

        return true;
    }

    return Update(configuration_);
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
    if(json_configuration_service_->IsAvailable()) {
        auto json_config = json_parser_->Serialize(*configuration);
        json_configuration_service_->Save(json_config.get());

        auto json_config_loaded = json_configuration_service_->Load();
        if(!json_config_loaded.has_value()) {
            LOG_ERR("Failed to load newly updated JSON configuration.");
            return false;
        }

        LOG_INF("JSON configuration updated successfully.");

        uint32_t crc = crc32_ieee(json_config_loaded->config_raw->data(), json_config_loaded->config_raw->size());
        json_config_checksum_ = crc;
    }

    auto cbor_config = cbor_parser_->Serialize(*configuration);
    cbor_config->json_config_checksum = json_config_checksum_;

    if(!cbor_configuration_service_->Save(cbor_config.get()))
        return false;

    Get(true);

    return true;
}

std::shared_ptr<SystemConfiguration> SystemConfigurationManager::Get(bool force_load) {
    if(configuration_ != nullptr && !force_load)
        return configuration_;

    auto cbor_config = cbor_configuration_service_->Load();
    if(!cbor_config.has_value())
        return nullptr;

    cbor_config_raw_ = std::move(cbor_config.value().config_raw);
    cbor_config_ = std::move(cbor_config.value().config);

    auto configuration = cbor_parser_->Deserialize(*cbor_config_);
    configuration_ = std::make_shared<SystemConfiguration>(configuration);

    json_config_checksum_ = cbor_config_->json_config_checksum;

    return configuration_;
}

} // namespace eerie_leap::domain::system_domain::configuration
