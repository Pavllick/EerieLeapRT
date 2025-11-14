#include <utility>

#include "canbus_configuration_validator.h"
#include "canbus_configuration_cbor_parser.h"

namespace eerie_leap::domain::canbus_domain::utilities::parsers {

ext_unique_ptr<CborCanbusConfig> CanbusConfigurationCborParser::Serialize(const CanbusConfiguration& configuration) {
    CanbusConfigurationValidator::Validate(configuration);

    auto config = make_unique_ext<CborCanbusConfig>();
    memset(config.get(), 0, sizeof(CborCanbusConfig));

    config->CborCanChannelConfig_m_count = 0;

    for(size_t i = 0; i < configuration.channel_configurations.size() && i < 8; i++) {
        config->CborCanChannelConfig_m[i].type = std::to_underlying(configuration.channel_configurations[i].type);
        config->CborCanChannelConfig_m[i].bus_channel = configuration.channel_configurations[i].bus_channel;
        config->CborCanChannelConfig_m[i].bitrate = configuration.channel_configurations[i].bitrate;

        config->CborCanChannelConfig_m[i].CborCanMessageConfig_m_count = 0;

        for(size_t j = 0; j < configuration.channel_configurations[i].message_configurations.size() && j < 24; j++) {
            config->CborCanChannelConfig_m[i].CborCanMessageConfig_m[j].frame_id = configuration.channel_configurations[i].message_configurations[j].frame_id;
            config->CborCanChannelConfig_m[i].CborCanMessageConfig_m[j].send_interval_ms = configuration.channel_configurations[i].message_configurations[j].send_interval_ms;

            config->CborCanChannelConfig_m[i].CborCanMessageConfig_m_count++;
        }

        config->CborCanChannelConfig_m_count++;
    }

    return config;
}

CanbusConfiguration CanbusConfigurationCborParser::Deserialize(const CborCanbusConfig& config) {
    CanbusConfiguration configuration;

    for(size_t i = 0; i < config.CborCanChannelConfig_m_count; ++i) {
        const auto& canbus_config = config.CborCanChannelConfig_m[i];
        configuration.channel_configurations.push_back({
            .type = static_cast<CanbusType>(canbus_config.type),
            .bus_channel = static_cast<uint8_t>(canbus_config.bus_channel),
            .bitrate = canbus_config.bitrate
        });

        for(size_t j = 0; j < canbus_config.CborCanMessageConfig_m_count; ++j) {
            configuration.channel_configurations[i].message_configurations.push_back({
                .frame_id = canbus_config.CborCanMessageConfig_m[j].frame_id,
                .send_interval_ms = canbus_config.CborCanMessageConfig_m[j].send_interval_ms
            });
        }
    }

    CanbusConfigurationValidator::Validate(configuration);

    return configuration;
}

} // namespace eerie_leap::domain::canbus_domain::utilities::parsers
