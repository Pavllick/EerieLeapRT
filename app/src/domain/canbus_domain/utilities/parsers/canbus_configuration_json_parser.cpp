#include "canbus_configuration_validator.h"
#include "canbus_configuration_json_parser.h"

namespace eerie_leap::domain::canbus_domain::utilities::parsers {

CanbusConfigurationJsonParser::CanbusConfigurationJsonParser(std::shared_ptr<IFsService> fs_service)
    : fs_service_(std::move(fs_service)) {}

ext_unique_ptr<JsonCanbusConfig> CanbusConfigurationJsonParser::Serialize(const CanbusConfiguration& configuration) {
    CanbusConfigurationValidator::Validate(configuration);

    auto config = make_unique_ext<JsonCanbusConfig>();

    for(const auto& [bus_channel, channel_configuration] : configuration.channel_configurations) {
        JsonCanChannelConfig channel_config {
            .type = GetCanbusTypeName(channel_configuration.type),
            .bus_channel = bus_channel,
            .bitrate = channel_configuration.bitrate,
            .dbc_file_path = channel_configuration.dbc_file_path,
            .script_path = channel_configuration.script_path
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
        CanChannelConfiguration channel_configuration = {
            .type = GetCanbusType(canbus_config.type),
            .bus_channel = static_cast<uint8_t>(canbus_config.bus_channel),
            .bitrate = canbus_config.bitrate,
            .dbc_file_path = canbus_config.dbc_file_path,
            .script_path = canbus_config.script_path
        };

        if(fs_service_ != nullptr
            && fs_service_->IsAvailable()
            && !channel_configuration.script_path.empty()
            && fs_service_->Exists(channel_configuration.script_path)) {

            size_t script_size = fs_service_->GetFileSize(channel_configuration.script_path);

            if(script_size != 0) {
                ext_unique_ptr<ExtVector> buffer = make_unique_ext<ExtVector>(script_size);

                size_t out_len = 0;
                fs_service_->ReadFile(channel_configuration.script_path, buffer->data(), script_size, out_len);

                channel_configuration.lua_script = std::make_shared<LuaScript>();
                channel_configuration.lua_script->Load(std::span<const uint8_t>(buffer->data(), buffer->size()));
            }
        }

        for(const auto& message_config : canbus_config.message_configs) {
            channel_configuration.message_configurations.push_back({
                .frame_id = message_config.frame_id,
                .send_interval_ms = message_config.send_interval_ms
            });
        }

        configuration.channel_configurations.emplace(canbus_config.bus_channel, channel_configuration);
    }

    CanbusConfigurationValidator::Validate(configuration);

    return configuration;
}

} // eerie_leap::domain::canbus_domain::utilities::parsers
