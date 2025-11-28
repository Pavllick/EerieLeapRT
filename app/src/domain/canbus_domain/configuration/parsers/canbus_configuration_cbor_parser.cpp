#include <utility>

#include "utilities/cbor/cbor_helpers.hpp"

#include "canbus_configuration_validator.h"
#include "canbus_configuration_cbor_parser.h"

namespace eerie_leap::domain::canbus_domain::configuration::parsers {

using namespace eerie_leap::utilities::cbor;

CanbusConfigurationCborParser::CanbusConfigurationCborParser(std::shared_ptr<IFsService> fs_service)
    : fs_service_(std::move(fs_service)) {}

ext_unique_ptr<CborCanbusConfig> CanbusConfigurationCborParser::Serialize(const CanbusConfiguration& configuration) {
    CanbusConfigurationValidator::Validate(configuration);

    auto config = make_unique_ext<CborCanbusConfig>();

    for(const auto& [bus_channel, channel_configuration] : configuration.channel_configurations) {
        CborCanChannelConfig channel_config = {
            .type = std::to_underlying(channel_configuration.type),
            .is_extended_id = channel_configuration.is_extended_id,
            .bus_channel = bus_channel,
            .bitrate = channel_configuration.bitrate,
            .data_bitrate = channel_configuration.data_bitrate,
            .dbc_file_path = CborHelpers::ToZcborString(&channel_configuration.dbc_file_path)
        };

        for(const auto& message_configuration : channel_configuration.message_configurations) {
            channel_config.CborCanMessageConfig_m.push_back({
                .frame_id = message_configuration.frame_id,
                .send_interval_ms = message_configuration.send_interval_ms,
                .script_path = CborHelpers::ToZcborString(&message_configuration.script_path)
            });
        }

        config->CborCanChannelConfig_m.push_back(channel_config);
    }

    return config;
}

CanbusConfiguration CanbusConfigurationCborParser::Deserialize(const CborCanbusConfig& config) {
    CanbusConfiguration configuration;

    for(const auto& canbus_config : config.CborCanChannelConfig_m) {
        CanChannelConfiguration channel_configuration = {
            .type = static_cast<CanbusType>(canbus_config.type),
            .is_extended_id = canbus_config.is_extended_id,
            .bus_channel = static_cast<uint8_t>(canbus_config.bus_channel),
            .bitrate = canbus_config.bitrate,
            .data_bitrate = canbus_config.data_bitrate,
            .dbc_file_path = CborHelpers::ToStdString(canbus_config.dbc_file_path)
        };

        for(const auto& message_config : canbus_config.CborCanMessageConfig_m) {
            CanMessageConfiguration message_configuration = {
                .frame_id = message_config.frame_id,
                .send_interval_ms = message_config.send_interval_ms,
                .script_path = CborHelpers::ToStdString(message_config.script_path)
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

        configuration.channel_configurations.emplace(channel_configuration.bus_channel, channel_configuration);
    }

    CanbusConfigurationValidator::Validate(configuration);

    return configuration;
}

} // namespace eerie_leap::domain::canbus_domain::configuration::parsers
