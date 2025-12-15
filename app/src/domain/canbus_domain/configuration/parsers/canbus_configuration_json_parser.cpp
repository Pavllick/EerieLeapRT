#include "canbus_configuration_validator.h"
#include "canbus_configuration_parser_helpers.hpp"
#include "canbus_configuration_json_parser.h"

namespace eerie_leap::domain::canbus_domain::configuration::parsers {

CanbusConfigurationJsonParser::CanbusConfigurationJsonParser(std::shared_ptr<IFsService> sd_fs_service)
    : sd_fs_service_(std::move(sd_fs_service)) {}

pmr_unique_ptr<JsonCanbusConfig> CanbusConfigurationJsonParser::Serialize(const CanbusConfiguration& configuration) {
    CanbusConfigurationValidator::Validate(configuration, sd_fs_service_.get());

    auto config = make_unique_pmr<JsonCanbusConfig>(Mrm::GetExtPmr());

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
            message_config.frame_id = message_configuration->frame_id;
            message_config.send_interval_ms = message_configuration->send_interval_ms;
            message_config.script_path = json::string(message_configuration->script_path);
            message_config.name = json::string(message_configuration->name);
            message_config.message_size = message_configuration->message_size;

            for(const auto& signal_configuration : message_configuration->signal_configurations) {
                JsonCanSignalConfig signal_config;
                signal_config.start_bit = signal_configuration.start_bit;
                signal_config.size_bits = signal_configuration.size_bits;
                signal_config.factor = signal_configuration.factor;
                signal_config.offset = signal_configuration.offset;
                signal_config.name = json::string(signal_configuration.name);
                signal_config.unit = json::string(signal_configuration.unit);

                message_config.signal_configs.push_back(std::move(signal_config));
            }

            channel_config.message_configs.push_back(std::move(message_config));
        }

        config->channel_configs.push_back(std::move(channel_config));
    }

    return config;
}

pmr_unique_ptr<CanbusConfiguration> CanbusConfigurationJsonParser::Deserialize(std::pmr::memory_resource* mr, const JsonCanbusConfig& config) {
    auto configuration = make_unique_pmr<CanbusConfiguration>(mr);

    for(const auto& canbus_config : config.channel_configs) {
        CanChannelConfiguration channel_configuration(std::allocator_arg, mr);

        channel_configuration.type = GetCanbusType(std::string(canbus_config.type));
        channel_configuration.is_extended_id = canbus_config.is_extended_id;
        channel_configuration.bus_channel = static_cast<uint8_t>(canbus_config.bus_channel);
        channel_configuration.bitrate = canbus_config.bitrate;
        channel_configuration.data_bitrate = canbus_config.data_bitrate;
        channel_configuration.dbc_file_path = std::string(canbus_config.dbc_file_path);

        if(sd_fs_service_ != nullptr && !channel_configuration.dbc_file_path.empty())
            CanbusConfigurationParserHelpers::LoadDbcConfiguration(sd_fs_service_.get(), channel_configuration);

        for(const auto& message_config : canbus_config.message_configs) {
            auto message_configuration = make_shared_pmr<CanMessageConfiguration>(mr);

            message_configuration->frame_id = message_config.frame_id;
            message_configuration->send_interval_ms = message_config.send_interval_ms;
            message_configuration->script_path = std::string(message_config.script_path);
            message_configuration->name = std::string(message_config.name);
            message_configuration->message_size = message_config.message_size;

            for(const auto& signal_config : message_config.signal_configs) {
                CanSignalConfiguration signal_configuration(std::allocator_arg, mr);

                signal_configuration.start_bit = signal_config.start_bit;
                signal_configuration.size_bits = signal_config.size_bits;
                signal_configuration.factor = signal_config.factor;
                signal_configuration.offset = signal_config.offset;
                signal_configuration.name = signal_config.name;
                signal_configuration.unit = signal_config.unit;

                message_configuration->signal_configurations.emplace_back(std::move(signal_configuration));
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

            channel_configuration.message_configurations.emplace_back(std::move(message_configuration));
        }

        bool res = configuration->channel_configurations.emplace(
            canbus_config.bus_channel,
            std::move(channel_configuration)).second;

        if(!res)
            throw std::runtime_error("Duplicate CAN bus channel " + std::to_string(canbus_config.bus_channel));
    }

    CanbusConfigurationValidator::Validate(*configuration, sd_fs_service_.get());

    return configuration;
}

} // eerie_leap::domain::canbus_domain::configuration::parsers
