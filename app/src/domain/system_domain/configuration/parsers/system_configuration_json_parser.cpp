#include "system_configuration_validator.h"
#include "utilities/memory/memory_resource.h"
#include "system_configuration_json_parser.h"

namespace eerie_leap::domain::system_domain::configuration::parsers {

pmr_unique_ptr<JsonSystemConfig> SystemConfigurationJsonParser::Serialize(const SystemConfiguration& configuration) {
    SystemConfigurationValidator::Validate(configuration);

    auto config = make_unique_pmr<JsonSystemConfig>(Mrm::GetExtPmr());

    config->com_user_refresh_rate_ms = configuration.com_user_refresh_rate_ms;

    for(auto& com_user_config : configuration.com_user_configurations) {
        config->com_user_configs.push_back({
            .device_id = com_user_config.device_id,
            .server_id = com_user_config.server_id
        });
    }

    return config;
}

pmr_unique_ptr<SystemConfiguration> SystemConfigurationJsonParser::Deserialize(
    std::pmr::memory_resource* mr,
    const JsonSystemConfig& config,
    uint64_t device_id,
    uint32_t hw_version,
    uint32_t sw_version,
    uint32_t build_number) {

    auto configuration = make_unique_pmr<SystemConfiguration>(mr);

    configuration->device_id = device_id;
    configuration->hw_version = hw_version;
    configuration->sw_version = sw_version;
    configuration->build_number = build_number;
    configuration->com_user_refresh_rate_ms = config.com_user_refresh_rate_ms;

    for(auto& com_user_config : config.com_user_configs) {
        configuration->com_user_configurations.push_back({
            .device_id = com_user_config.device_id,
            .server_id = static_cast<uint16_t>(com_user_config.server_id)
        });
    }

    SystemConfigurationValidator::Validate(*configuration);

    return configuration;
}

} // eerie_leap::domain::system_domain::configuration::parsers
