#pragma once

#include <vector>
#include <string>
#include <boost/json.hpp>
#include <nameof.hpp>

namespace eerie_leap::configuration::json::configs {

namespace json = boost::json;

struct JsonCanMessageConfig {
    uint32_t frame_id;
    uint32_t send_interval_ms;
    std::string script_path;
};

struct JsonCanChannelConfig {
    std::string type;
    bool is_extended_id;
    uint32_t bus_channel;
    uint32_t bitrate;
    uint32_t data_bitrate;
    std::string dbc_file_path;
    std::vector<JsonCanMessageConfig> message_configs;
};

struct JsonCanbusConfig {
    std::vector<JsonCanChannelConfig> channel_configs;
};

static void tag_invoke(json::value_from_tag, json::value& jv, JsonCanMessageConfig const& config) {
    jv = {
        {NAMEOF_MEMBER(&JsonCanMessageConfig::frame_id), config.frame_id},
        {NAMEOF_MEMBER(&JsonCanMessageConfig::send_interval_ms), config.send_interval_ms},
        {NAMEOF_MEMBER(&JsonCanMessageConfig::script_path), config.script_path}
    };
}

static JsonCanMessageConfig tag_invoke(json::value_to_tag<JsonCanMessageConfig>, json::value const& jv) {
    json::object const& obj = jv.as_object();
    return {
        .frame_id = json::value_to<uint32_t>(obj.at(NAMEOF_MEMBER(&JsonCanMessageConfig::frame_id).c_str())),
        .send_interval_ms = json::value_to<uint32_t>(obj.at(NAMEOF_MEMBER(&JsonCanMessageConfig::send_interval_ms).c_str())),
        .script_path = json::value_to<std::string>(obj.at(NAMEOF_MEMBER(&JsonCanMessageConfig::script_path).c_str()))
    };
}

static void tag_invoke(json::value_from_tag, json::value& jv, JsonCanChannelConfig const& config) {
    jv = {
        {NAMEOF_MEMBER(&JsonCanChannelConfig::type), config.type},
        {NAMEOF_MEMBER(&JsonCanChannelConfig::is_extended_id), config.is_extended_id},
        {NAMEOF_MEMBER(&JsonCanChannelConfig::bus_channel), config.bus_channel},
        {NAMEOF_MEMBER(&JsonCanChannelConfig::bitrate), config.bitrate},
        {NAMEOF_MEMBER(&JsonCanChannelConfig::data_bitrate), config.data_bitrate},
        {NAMEOF_MEMBER(&JsonCanChannelConfig::dbc_file_path), config.dbc_file_path},
        {NAMEOF_MEMBER(&JsonCanChannelConfig::message_configs), json::value_from(config.message_configs)}
    };
}

static JsonCanChannelConfig tag_invoke(json::value_to_tag<JsonCanChannelConfig>, json::value const& jv) {
    json::object const& obj = jv.as_object();
    return {
        .type = json::value_to<std::string>(obj.at(NAMEOF_MEMBER(&JsonCanChannelConfig::type).c_str())),
        .is_extended_id = json::value_to<bool>(obj.at(NAMEOF_MEMBER(&JsonCanChannelConfig::is_extended_id).c_str())),
        .bus_channel = json::value_to<uint32_t>(obj.at(NAMEOF_MEMBER(&JsonCanChannelConfig::bus_channel).c_str())),
        .bitrate = json::value_to<uint32_t>(obj.at(NAMEOF_MEMBER(&JsonCanChannelConfig::bitrate).c_str())),
        .data_bitrate = json::value_to<uint32_t>(obj.at(NAMEOF_MEMBER(&JsonCanChannelConfig::data_bitrate).c_str())),
        .dbc_file_path = json::value_to<std::string>(obj.at(NAMEOF_MEMBER(&JsonCanChannelConfig::dbc_file_path).c_str())),
        .message_configs = json::value_to<std::vector<JsonCanMessageConfig>>(obj.at(NAMEOF_MEMBER(&JsonCanChannelConfig::message_configs).c_str()))
    };
}

static void tag_invoke(json::value_from_tag, json::value& jv, JsonCanbusConfig const& config) {
    jv = {
        {NAMEOF_MEMBER(&JsonCanbusConfig::channel_configs), json::value_from(config.channel_configs)}
    };
}

static JsonCanbusConfig tag_invoke(json::value_to_tag<JsonCanbusConfig>, json::value const& jv) {
    json::object const& obj = jv.as_object();
    return {
        .channel_configs = json::value_to<std::vector<JsonCanChannelConfig>>(obj.at(NAMEOF_MEMBER(&JsonCanbusConfig::channel_configs).c_str()))
    };
}

static std::string json_encode_JsonCanbusConfig(const JsonCanbusConfig& config) {
    return json::serialize(json::value_from(config));
}

static JsonCanbusConfig json_decode_JsonCanbusConfig(std::string_view json_str) {
    json::value jv = json::parse(json_str);

    return json::value_to<JsonCanbusConfig>(jv);
}

} // namespace eerie_leap::configuration::json::configs
