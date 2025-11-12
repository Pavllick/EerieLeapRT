#include "system_configuration_json_parser.h"

namespace eerie_leap::domain::system_domain::utilities::parsers {

ext_unique_ptr<JsonSystemConfig> SystemConfigurationJsonParser::Serialize(const SystemConfiguration& system_configuration) {
    auto config = make_unique_ext<JsonSystemConfig>();
    memset(config.get(), 0, sizeof(JsonSystemConfig));

    config->com_user_refresh_rate_ms = system_configuration.com_user_refresh_rate_ms;

    config->com_user_configs_len = 0;

    for(size_t i = 0; i < system_configuration.com_user_configurations.size() && i < 8; i++) {
        config->com_user_configs[i].device_id = system_configuration.com_user_configurations[i].device_id;
        config->com_user_configs[i].server_id = system_configuration.com_user_configurations[i].server_id;

        config->com_user_configs_len++;
    }

    config->canbus_configs_len = 0;

    for(size_t i = 0; i < system_configuration.canbus_configurations.size() && i < 8; i++) {
        config->canbus_configs[i].bus_channel = system_configuration.canbus_configurations[i].bus_channel;
        config->canbus_configs[i].bitrate = system_configuration.canbus_configurations[i].bitrate;

        config->canbus_configs_len++;
    }

    return config;
}

SystemConfiguration SystemConfigurationJsonParser::Deserialize(
    const JsonSystemConfig& config,
    uint64_t device_id,
    uint32_t hw_version,
    uint32_t sw_version,
    uint32_t build_number) {

    SystemConfiguration system_configuration;

    system_configuration.device_id = device_id;
    system_configuration.hw_version = hw_version;
    system_configuration.sw_version = sw_version;
    system_configuration.build_number = build_number;
    system_configuration.com_user_refresh_rate_ms = config.com_user_refresh_rate_ms;

    for(size_t i = 0; i < config.com_user_configs_len; ++i) {
        const auto& com_user_config = config.com_user_configs[i];
        system_configuration.com_user_configurations.push_back({
            .device_id = com_user_config.device_id,
            .server_id = static_cast<uint16_t>(com_user_config.server_id)
        });
    }

    for(size_t i = 0; i < config.canbus_configs_len; ++i) {
        const auto& canbus_config = config.canbus_configs[i];
        system_configuration.canbus_configurations.push_back({
            .bus_channel = static_cast<uint8_t>(canbus_config.bus_channel),
            .bitrate = canbus_config.bitrate
        });
    }

    return system_configuration;
}

} // eerie_leap::domain::system_domain::utilities::parsers
