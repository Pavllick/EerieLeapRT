#include <utility>

#include "system_configuration_validator.h"
#include "system_configuration_cbor_parser.h"

namespace eerie_leap::domain::system_domain::configuration::utilities::parsers {

ext_unique_ptr<CborSystemConfig> SystemConfigurationCborParser::Serialize(const SystemConfiguration& configuration) {
    SystemConfigurationValidator::Validate(configuration);

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

    return config;
}

SystemConfiguration SystemConfigurationCborParser::Deserialize(const CborSystemConfig& config) {
    SystemConfiguration configuration;

    configuration.device_id = config.device_id;
    configuration.hw_version = config.hw_version;
    configuration.sw_version = config.sw_version;
    configuration.build_number = config.build_number;
    configuration.com_user_refresh_rate_ms = config.com_user_refresh_rate_ms;

    for(size_t i = 0; i < config.CborComUserConfig_m_count; ++i) {
        const auto& com_user_config = config.CborComUserConfig_m[i];
        configuration.com_user_configurations.push_back({
            .device_id = com_user_config.device_id,
            .server_id = static_cast<uint16_t>(com_user_config.server_id)
        });
    }

    SystemConfigurationValidator::Validate(configuration);

    return configuration;
}

} // namespace eerie_leap::domain::system_domain::configuration::utilities::parsers
