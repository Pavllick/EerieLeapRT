#include "subsys/random/rng.h"

#include "system_configuration_manager.h"

namespace eerie_leap::domain::system_domain::configuration {

using namespace eerie_leap::subsys::random;

LOG_MODULE_REGISTER(system_config_ctrl_logger);

SystemConfigurationManager::SystemConfigurationManager(ext_unique_ptr<ConfigurationService<SystemConfig>> system_configuration_service) :
    system_configuration_service_(std::move(system_configuration_service)),
    system_config_(nullptr),
    system_configuration_(nullptr) {

    auto system_config = Get(true);

    if(system_config == nullptr) {
        if(!CreateDefaultSystemConfiguration()) {
            LOG_ERR("Failed to create default System configuration.");
            return;
        }

        system_config = Get();
    }

    UpdateHwVersion(system_config->hw_version);
    UpdateSwVersion(system_config->sw_version);

    system_config = Get();

    LOG_INF("System Configuration Controller initialized successfully.");

    LOG_INF("HW Version: %s, SW Version: %s",
        system_config->GetFormattedHwVersion().c_str(), system_config->GetFormattedSwVersion().c_str());
    LOG_INF("Device ID: %llu", system_config->device_id);
}

bool SystemConfigurationManager::UpdateBuildNumber(uint32_t build_number) {
    auto system_configuration = Get();
    if(system_configuration == nullptr)
        return false;

    if(build_number != system_configuration->build_number) {
        system_configuration->build_number = build_number;

        bool result = Update(system_configuration);
        if(!result)
            return false;

        LOG_INF("Build number updated to %u.", system_configuration->build_number);
    }

    return true;
}

bool SystemConfigurationManager::UpdateComUsers(const std::vector<ComUserConfiguration>& com_users) {
    auto system_configuration = Get();
    if(system_configuration == nullptr)
        return false;

    system_configuration->com_users = com_users;

    bool result = Update(system_configuration);
    if(!result)
        return false;

    LOG_INF("Com Users updated. Total count: %zu.", system_configuration->com_users.size());

    return true;
}

bool SystemConfigurationManager::UpdateHwVersion(uint32_t hw_version) {
    auto system_configuration = Get();
    if(system_configuration == nullptr)
        return false;

    uint32_t config_hw_version = SystemConfiguration::GetConfigHwVersion();
    if(hw_version != config_hw_version) {
        system_configuration->hw_version = config_hw_version;

        bool result = Update(system_configuration);
        if(!result)
            return false;

        LOG_INF("HW version updated to %s.", system_configuration->GetFormattedHwVersion().c_str());
    }

    return true;
}

bool SystemConfigurationManager::UpdateSwVersion(uint32_t sw_version) {
    auto system_configuration = Get();
    if(system_configuration == nullptr)
        return false;

    uint32_t config_sw_version = SystemConfiguration::GetConfigSwVersion();
    if(sw_version != config_sw_version) {
        system_configuration->sw_version = config_sw_version;
        system_configuration->build_number = 0;

        bool result = Update(system_configuration);
        if(!result)
            return false;

        LOG_INF("SW version updated to %s.", system_configuration->GetFormattedSwVersion().c_str());
    }

    return true;
}

bool SystemConfigurationManager::CreateDefaultSystemConfiguration() {
    auto system_config = make_shared_ext<SystemConfiguration>();
    system_config->device_id = Rng::Get64(true);
    system_config->hw_version = 0;
    system_config->sw_version = 0;
    system_config->build_number = 0;

    return Update(system_config);
}

bool SystemConfigurationManager::Update(std::shared_ptr<SystemConfiguration> system_configuration) {
    SystemConfig system_config {
        .device_id = system_configuration->device_id,
        .hw_version = system_configuration->hw_version,
        .sw_version = system_configuration->sw_version,
        .build_number = system_configuration->build_number
    };

    system_config.ComUserConfig_m_count = 0;
    memset(system_config.ComUserConfig_m, 0, sizeof(system_config.ComUserConfig_m));

    for(size_t i = 0; i < system_configuration->com_users.size() && i < 24; i++) {
        system_config.ComUserConfig_m[i].device_id = system_configuration->com_users[i].device_id;
        system_config.ComUserConfig_m[i].server_id = system_configuration->com_users[i].server_id;

        system_config.ComUserConfig_m_count++;
    }

    if(!system_configuration_service_->Save(&system_config))
        return false;

    system_config_ = make_unique_ext<SystemConfig>(system_config);
    system_configuration_ = system_configuration;

    return true;
}

std::shared_ptr<SystemConfiguration> SystemConfigurationManager::Get(bool force_load) {
    if (system_configuration_ != nullptr && !force_load) {
        return system_configuration_;
    }

    auto system_config = system_configuration_service_->Load();
    if(!system_config.has_value())
        return nullptr;

    system_config_raw_ = std::move(system_config.value().config_raw);
    system_config_ = std::move(system_config.value().config);

    SystemConfiguration system_configuration {
        .device_id = system_config_->device_id,
        .hw_version = system_config_->hw_version,
        .sw_version = system_config_->sw_version,
        .build_number = system_config_->build_number
    };

    for(size_t i = 0; i < system_config_->ComUserConfig_m_count; i++) {
        ComUserConfiguration com_user {
            .device_id = system_config_->ComUserConfig_m[i].device_id,
            .server_id = system_config_->ComUserConfig_m[i].server_id
        };
        system_configuration.com_users.push_back(com_user);
    }

    system_configuration_ = make_shared_ext<SystemConfiguration>(system_configuration);

    return system_configuration_;
}

} // namespace eerie_leap::domain::system_domain::configuration
