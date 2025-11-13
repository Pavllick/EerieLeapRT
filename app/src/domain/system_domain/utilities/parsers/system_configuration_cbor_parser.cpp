#include <utility>

#include "system_configuration_validator.h"
#include "system_configuration_cbor_parser.h"

namespace eerie_leap::domain::system_domain::utilities::parsers {

ext_unique_ptr<CborSystemConfig> SystemConfigurationCborParser::Serialize(const SystemConfiguration& configuration) {
    SystemConfigurationValidator::ValidateSystemConfiguration(configuration);

    auto config = make_unique_ext<CborSystemConfig>();
    memset(config.get(), 0, sizeof(CborSystemConfig));

    config->device_id = configuration.device_id;
    config->hw_version = configuration.hw_version;
    config->sw_version = configuration.sw_version;
    config->build_number = configuration.build_number;
    config->com_user_refresh_rate_ms = configuration.com_user_refresh_rate_ms;

    config->CborComUserConfig_m_count = 0;

    for(size_t i = 0; i < configuration.com_user_configurations.size() && i < 8; i++) {
        config->CborComUserConfig_m[i].device_id = configuration.com_user_configurations[i].device_id;
        config->CborComUserConfig_m[i].server_id = configuration.com_user_configurations[i].server_id;

        config->CborComUserConfig_m_count++;
    }

    config->CborCanbusConfig_m_count = 0;

    for(size_t i = 0; i < configuration.canbus_configurations.size() && i < 8; i++) {
        config->CborCanbusConfig_m[i].type = std::to_underlying(configuration.canbus_configurations[i].type);
        config->CborCanbusConfig_m[i].bus_channel = configuration.canbus_configurations[i].bus_channel;
        config->CborCanbusConfig_m[i].bitrate = configuration.canbus_configurations[i].bitrate;

        config->CborCanbusConfig_m_count++;
    }

    return config;
}

SystemConfiguration SystemConfigurationCborParser::Deserialize(const CborSystemConfig& system_config) {
    SystemConfiguration configuration;

    configuration.device_id = system_config.device_id;
    configuration.hw_version = system_config.hw_version;
    configuration.sw_version = system_config.sw_version;
    configuration.build_number = system_config.build_number;
    configuration.com_user_refresh_rate_ms = system_config.com_user_refresh_rate_ms;

    for(size_t i = 0; i < system_config.CborComUserConfig_m_count; ++i) {
        const auto& com_user_config = system_config.CborComUserConfig_m[i];
        configuration.com_user_configurations.push_back({
            .device_id = com_user_config.device_id,
            .server_id = static_cast<uint16_t>(com_user_config.server_id)
        });
    }

    for(size_t i = 0; i < system_config.CborCanbusConfig_m_count; ++i) {
        const auto& canbus_config = system_config.CborCanbusConfig_m[i];
        configuration.canbus_configurations.push_back({
            .type = static_cast<CanbusType>(canbus_config.type),
            .bus_channel = static_cast<uint8_t>(canbus_config.bus_channel),
            .bitrate = canbus_config.bitrate
        });
    }

    SystemConfigurationValidator::ValidateSystemConfiguration(configuration);

    return configuration;
}

} // namespace eerie_leap::domain::system_domain::utilities::parsers
