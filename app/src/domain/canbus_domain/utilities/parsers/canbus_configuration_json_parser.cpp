#include "canbus_configuration_validator.h"
#include "canbus_configuration_json_parser.h"

namespace eerie_leap::domain::canbus_domain::utilities::parsers {

ext_unique_ptr<JsonCanbusConfig> CanbusConfigurationJsonParser::Serialize(const CanbusConfiguration& configuration) {
    CanbusConfigurationValidator::Validate(configuration);

    auto config = make_unique_ext<JsonCanbusConfig>();
    memset(config.get(), 0, sizeof(JsonCanbusConfig));

    config->channel_configs_len = 0;

    for(size_t i = 0; i < configuration.channel_configurations.size() && i < 8; i++) {
        config->channel_configs[i].type = GetCanbusTypeName(configuration.channel_configurations[i].type);
        config->channel_configs[i].bus_channel = configuration.channel_configurations[i].bus_channel;
        config->channel_configs[i].bitrate = configuration.channel_configurations[i].bitrate;

        config->channel_configs_len++;

        config->channel_configs[i].message_configs_len = 0;

        for(size_t j = 0; j < configuration.channel_configurations[i].message_configurations.size() && j < 24; j++) {
            config->channel_configs[i].message_configs[j].frame_id = configuration.channel_configurations[i].message_configurations[j].frame_id;
            config->channel_configs[i].message_configs[j].send_interval_ms = configuration.channel_configurations[i].message_configurations[j].send_interval_ms;

            config->channel_configs[i].message_configs_len++;
        }
    }

    return config;
}

CanbusConfiguration CanbusConfigurationJsonParser::Deserialize(const JsonCanbusConfig& config) {
    CanbusConfiguration configuration;

    for(size_t i = 0; i < config.channel_configs_len; ++i) {
        const auto& canbus_config = config.channel_configs[i];
        configuration.channel_configurations.push_back({
            .type = GetCanbusType(canbus_config.type),
            .bus_channel = static_cast<uint8_t>(canbus_config.bus_channel),
            .bitrate = canbus_config.bitrate
        });

        for(size_t j = 0; j < canbus_config.message_configs_len; ++j) {
            const auto& message_config = canbus_config.message_configs[j];
            configuration.channel_configurations[i].message_configurations.push_back({
                .frame_id = message_config.frame_id,
                .send_interval_ms = message_config.send_interval_ms
            });
        }
    }

    CanbusConfigurationValidator::Validate(configuration);

    return configuration;
}

} // eerie_leap::domain::canbus_domain::utilities::parsers
