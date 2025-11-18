#include "canbus_configuration_validator.h"
#include "canbus_configuration_json_parser.h"

namespace eerie_leap::domain::canbus_domain::utilities::parsers {

ext_unique_ptr<JsonCanbusConfig> CanbusConfigurationJsonParser::Serialize(const CanbusConfiguration& configuration) {
    CanbusConfigurationValidator::Validate(configuration);

    auto config = make_unique_ext<JsonCanbusConfig>();

    for(const auto& channel_configuration : configuration.channel_configurations) {
        JsonCanChannelConfig channel_config {
            .type = GetCanbusTypeName(channel_configuration.type),
            .bus_channel = channel_configuration.bus_channel,
            .bitrate = channel_configuration.bitrate
        };

        for(const auto& message_configuration : channel_configuration.message_configurations) {
            channel_config.message_configs.push_back({
                .frame_id = message_configuration.frame_id,
                .send_interval_ms = message_configuration.send_interval_ms
            });
        }

        config->channel_configs.push_back(channel_config);
    }

    return config;
}

CanbusConfiguration CanbusConfigurationJsonParser::Deserialize(const JsonCanbusConfig& config) {
    CanbusConfiguration configuration;

    for(const auto& canbus_config : config.channel_configs) {
        configuration.channel_configurations.push_back({
            .type = GetCanbusType(canbus_config.type),
            .bus_channel = static_cast<uint8_t>(canbus_config.bus_channel),
            .bitrate = canbus_config.bitrate
        });

        for(const auto& message_config : canbus_config.message_configs) {
            configuration.channel_configurations.back().message_configurations.push_back({
                .frame_id = message_config.frame_id,
                .send_interval_ms = message_config.send_interval_ms
            });
        }
    }

    CanbusConfigurationValidator::Validate(configuration);

    return configuration;
}

} // eerie_leap::domain::canbus_domain::utilities::parsers
