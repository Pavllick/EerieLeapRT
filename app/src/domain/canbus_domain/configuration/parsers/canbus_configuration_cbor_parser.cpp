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
            .dbc_file_path = CborHelpers::ToZcborString(channel_configuration.dbc_file_path)
        };

        for(const auto& message_configuration : channel_configuration.message_configurations) {
            CborCanMessageConfig message_config = {
                .frame_id = message_configuration.frame_id,
                .send_interval_ms = message_configuration.send_interval_ms,
                .script_path = CborHelpers::ToZcborString(message_configuration.script_path),
                .name = CborHelpers::ToZcborString(message_configuration.name),
                .message_size = message_configuration.message_size
            };

            for(const auto& signal_configuration : message_configuration.signal_configurations) {
                message_config.CborCanSignalConfig_m.push_back({
                    .start_bit = signal_configuration.start_bit,
                    .size_bits = signal_configuration.size_bits,
                    .factor = signal_configuration.factor,
                    .offset = signal_configuration.offset,
                    .name = CborHelpers::ToZcborString(signal_configuration.name),
                    .unit = CborHelpers::ToZcborString(signal_configuration.unit)
                });
            }

            channel_config.CborCanMessageConfig_m.push_back(message_config);
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
                .script_path = CborHelpers::ToStdString(message_config.script_path),
                .name = CborHelpers::ToStdString(message_config.name),
                .message_size = message_config.message_size
            };

            for(const auto& signal_config : message_config.CborCanSignalConfig_m) {
                message_configuration.signal_configurations.push_back({
                    .start_bit = signal_config.start_bit,
                    .size_bits = signal_config.size_bits,
                    .factor = signal_config.factor,
                    .offset = signal_config.offset,
                    .name = CborHelpers::ToStdString(signal_config.name),
                    .unit = CborHelpers::ToStdString(signal_config.unit)
                });
            }

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
