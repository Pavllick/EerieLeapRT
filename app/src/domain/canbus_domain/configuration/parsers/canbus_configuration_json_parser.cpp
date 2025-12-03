#include "canbus_configuration_validator.h"
#include "canbus_configuration_json_parser.h"

namespace eerie_leap::domain::canbus_domain::configuration::parsers {

CanbusConfigurationJsonParser::CanbusConfigurationJsonParser(std::shared_ptr<IFsService> fs_service)
    : fs_service_(std::move(fs_service)) {}

ext_unique_ptr<JsonCanbusConfig> CanbusConfigurationJsonParser::Serialize(const CanbusConfiguration& configuration) {
    CanbusConfigurationValidator::Validate(configuration);

    auto config = make_unique_ext<JsonCanbusConfig>();

    for(const auto& [bus_channel, channel_configuration] : configuration.channel_configurations) {
        JsonCanChannelConfig channel_config;
        channel_config.type = json::string(GetCanbusTypeName(channel_configuration.type));
        channel_config.is_extended_id = channel_configuration.is_extended_id;
        channel_config.bus_channel = bus_channel;
        channel_config.bitrate = channel_configuration.bitrate;
        channel_config.data_bitrate = channel_configuration.data_bitrate;
        channel_config.dbc_file_path = json::string(channel_configuration.dbc_file_path);

        for(const auto& message_configuration : channel_configuration.message_configurations) {
            JsonCanMessageConfig message_config;
            message_config.frame_id = message_configuration.frame_id;
            message_config.send_interval_ms = message_configuration.send_interval_ms;
            message_config.script_path = json::string(message_configuration.script_path);

            channel_config.message_configs.push_back(std::move(message_config));
        }

        config->channel_configs.push_back(std::move(channel_config));
    }

    return config;
}

CanbusConfiguration CanbusConfigurationJsonParser::Deserialize(const JsonCanbusConfig& config) {
    CanbusConfiguration configuration;

    for(const auto& canbus_config : config.channel_configs) {
        CanChannelConfiguration channel_configuration = {
            .type = GetCanbusType(std::string(canbus_config.type)),
            .is_extended_id = canbus_config.is_extended_id,
            .bus_channel = static_cast<uint8_t>(canbus_config.bus_channel),
            .bitrate = canbus_config.bitrate,
            .data_bitrate = canbus_config.data_bitrate,
            .dbc_file_path = std::string(canbus_config.dbc_file_path)
        };

        for(const auto& message_config : canbus_config.message_configs) {
            CanMessageConfiguration message_configuration = {
                .frame_id = message_config.frame_id,
                .send_interval_ms = message_config.send_interval_ms,
                .script_path = std::string(message_config.script_path)
            };

            if(fs_service_ != nullptr
                && fs_service_->IsAvailable()
                && !message_configuration.script_path.empty()
                && fs_service_->Exists(message_configuration.script_path)) {

                size_t script_size = fs_service_->GetFileSize(message_configuration.script_path);

                if(script_size != 0) {
                    ext_unique_ptr<ExtVector> buffer = make_unique_ext<ExtVector>(script_size);

                    size_t out_len = 0;
                    fs_service_->ReadFile(message_configuration.script_path, buffer->data(), script_size, out_len);

                    message_configuration.lua_script = std::make_shared<LuaScript>(LuaScript::CreateExt());
                    message_configuration.lua_script->Load(std::span<const uint8_t>(buffer->data(), buffer->size()));
                }
            }

            channel_configuration.message_configurations.push_back(message_configuration);
        }

        configuration.channel_configurations.emplace(canbus_config.bus_channel, channel_configuration);
    }

    CanbusConfigurationValidator::Validate(configuration);

    return configuration;
}

} // eerie_leap::domain::canbus_domain::configuration::parsers
