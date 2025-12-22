#include <utility>

#include "utilities/cbor/cbor_helpers.hpp"

#include "canbus_configuration_validator.h"
#include "canbus_configuration_parser_helpers.hpp"
#include "canbus_configuration_cbor_parser.h"

namespace eerie_leap::domain::canbus_domain::configuration::parsers {

using namespace eerie_leap::utilities::cbor;

CanbusConfigurationCborParser::CanbusConfigurationCborParser(std::shared_ptr<IFsService> sd_fs_service)
    : sd_fs_service_(std::move(sd_fs_service)) {}

pmr_unique_ptr<CborCanbusConfig> CanbusConfigurationCborParser::Serialize(const CanbusConfiguration& configuration) {
    CanbusConfigurationValidator::Validate(configuration, sd_fs_service_.get());

    auto config = make_unique_pmr<CborCanbusConfig>(Mrm::GetExtPmr());

    for(const auto& [bus_channel, channel_configuration] : configuration.channel_configurations) {
        CborCanChannelConfig channel_config(std::allocator_arg, Mrm::GetExtPmr());

        channel_config.type = std::to_underlying(channel_configuration.type);
        channel_config.is_extended_id = channel_configuration.is_extended_id;
        channel_config.bus_channel = bus_channel;
        channel_config.bitrate = channel_configuration.bitrate;
        channel_config.data_bitrate = channel_configuration.data_bitrate;
        channel_config.dbc_file_path = CborHelpers::ToZcborString(channel_configuration.dbc_file_path);

        for(const auto& message_configuration : channel_configuration.message_configurations) {
            CborCanMessageConfig message_config(std::allocator_arg, Mrm::GetExtPmr());

            message_config.frame_id = message_configuration->frame_id;
            message_config.send_interval_ms = message_configuration->send_interval_ms;
            message_config.script_path = CborHelpers::ToZcborString(message_configuration->script_path);
            message_config.name = CborHelpers::ToZcborString(message_configuration->name);
            message_config.message_size = message_configuration->message_size;

            for(const auto& signal_configuration : message_configuration->signal_configurations) {
                message_config.CborCanSignalConfig_m.push_back({
                    .start_bit = signal_configuration.start_bit,
                    .size_bits = signal_configuration.size_bits,
                    .factor = signal_configuration.factor,
                    .offset = signal_configuration.offset,
                    .name = CborHelpers::ToZcborString(signal_configuration.name),
                    .unit = CborHelpers::ToZcborString(signal_configuration.unit)
                });
            }

            channel_config.CborCanMessageConfig_m.push_back(std::move(message_config));
        }

        config->CborCanChannelConfig_m.push_back(std::move(channel_config));
    }

    return config;
}

pmr_unique_ptr<CanbusConfiguration> CanbusConfigurationCborParser::Deserialize(std::pmr::memory_resource* mr, const CborCanbusConfig& config) {
    auto configuration = make_unique_pmr<CanbusConfiguration>(mr);

    for(const auto& canbus_config : config.CborCanChannelConfig_m) {
        CanChannelConfiguration channel_configuration(std::allocator_arg, mr);

        channel_configuration.type = static_cast<CanbusType>(canbus_config.type);
        channel_configuration.is_extended_id = canbus_config.is_extended_id;
        channel_configuration.bus_channel = static_cast<uint8_t>(canbus_config.bus_channel);
        channel_configuration.bitrate = canbus_config.bitrate;
        channel_configuration.data_bitrate = canbus_config.data_bitrate;
        channel_configuration.dbc_file_path = CborHelpers::ToPmrString(mr, canbus_config.dbc_file_path);

        if(sd_fs_service_ != nullptr && !channel_configuration.dbc_file_path.empty())
            CanbusConfigurationParserHelpers::LoadDbcConfiguration(sd_fs_service_.get(), channel_configuration);

        for(const auto& message_config : canbus_config.CborCanMessageConfig_m) {
            auto message_configuration = make_shared_pmr<CanMessageConfiguration>(mr);

            message_configuration->frame_id = message_config.frame_id;
            message_configuration->send_interval_ms = message_config.send_interval_ms;
            message_configuration->script_path = CborHelpers::ToPmrString(mr, message_config.script_path);
            message_configuration->name = CborHelpers::ToPmrString(mr, message_config.name);
            message_configuration->message_size = message_config.message_size;

            for(const auto& signal_config : message_config.CborCanSignalConfig_m) {
                CanSignalConfiguration signal_configuration(std::allocator_arg, mr);

                signal_configuration.start_bit = signal_config.start_bit;
                signal_configuration.size_bits = signal_config.size_bits;
                signal_configuration.factor = signal_config.factor;
                signal_configuration.offset = signal_config.offset;
                signal_configuration.name = CborHelpers::ToPmrString(mr, signal_config.name);
                signal_configuration.unit = CborHelpers::ToPmrString(mr, signal_config.unit);

                message_configuration->signal_configurations.push_back(std::move(signal_configuration));
            }

            if(sd_fs_service_ != nullptr
                && sd_fs_service_->IsAvailable()
                && !message_configuration->script_path.empty()
                && sd_fs_service_->Exists(message_configuration->script_path)) {

                size_t script_size = sd_fs_service_->GetFileSize(message_configuration->script_path);

                if(script_size != 0) {
                    std::pmr::vector<uint8_t> buffer(script_size, Mrm::GetExtPmr());

                    size_t out_len = 0;
                    sd_fs_service_->ReadFile(message_configuration->script_path, buffer.data(), script_size, out_len);

                    message_configuration->lua_script = std::make_shared<LuaScript>(LuaScript::CreateExt());
                    message_configuration->lua_script->Load(std::span<const uint8_t>(buffer.data(), buffer.size()));
                }
            }

            channel_configuration.message_configurations.push_back(std::move(message_configuration));
        }

        bool res = configuration->channel_configurations.emplace(
            channel_configuration.bus_channel,
            std::move(channel_configuration)).second;

        if(!res)
            throw std::runtime_error("Duplicate CAN bus channel " + std::to_string(channel_configuration.bus_channel));
    }

    CanbusConfigurationValidator::Validate(*configuration, sd_fs_service_.get());

    return configuration;
}

} // namespace eerie_leap::domain::canbus_domain::configuration::parsers
