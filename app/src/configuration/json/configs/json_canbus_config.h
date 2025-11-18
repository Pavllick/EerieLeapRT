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
};

struct JsonCanChannelConfig {
    std::string type;
    uint32_t bus_channel;
    uint32_t bitrate;
    std::vector<JsonCanMessageConfig> message_configs;
};

struct JsonCanbusConfig {
    std::vector<JsonCanChannelConfig> channel_configs;
};

static void tag_invoke(json::value_from_tag, json::value& jv, JsonCanMessageConfig const& config) {
    jv = {
        {NAMEOF_MEMBER(&JsonCanMessageConfig::frame_id), config.frame_id},
        {NAMEOF_MEMBER(&JsonCanMessageConfig::send_interval_ms), config.send_interval_ms}
    };
}

static JsonCanMessageConfig tag_invoke(json::value_to_tag<JsonCanMessageConfig>, json::value const& jv) {
    json::object const& obj = jv.as_object();
    return JsonCanMessageConfig{
        json::value_to<uint32_t>(obj.at(NAMEOF_MEMBER(&JsonCanMessageConfig::frame_id).c_str())),
        json::value_to<uint32_t>(obj.at(NAMEOF_MEMBER(&JsonCanMessageConfig::send_interval_ms).c_str()))
    };
}

static void tag_invoke(json::value_from_tag, json::value& jv, JsonCanChannelConfig const& config) {
    jv = {
        {NAMEOF_MEMBER(&JsonCanChannelConfig::type), config.type},
        {NAMEOF_MEMBER(&JsonCanChannelConfig::bus_channel), config.bus_channel},
        {NAMEOF_MEMBER(&JsonCanChannelConfig::bitrate), config.bitrate},
        {NAMEOF_MEMBER(&JsonCanChannelConfig::message_configs), json::value_from(config.message_configs)}
    };
}

static JsonCanChannelConfig tag_invoke(json::value_to_tag<JsonCanChannelConfig>, json::value const& jv) {
    json::object const& obj = jv.as_object();
    return JsonCanChannelConfig{
        json::value_to<std::string>(obj.at(NAMEOF_MEMBER(&JsonCanChannelConfig::type).c_str())),
        json::value_to<uint32_t>(obj.at(NAMEOF_MEMBER(&JsonCanChannelConfig::bus_channel).c_str())),
        json::value_to<uint32_t>(obj.at(NAMEOF_MEMBER(&JsonCanChannelConfig::bitrate).c_str())),
        json::value_to<std::vector<JsonCanMessageConfig>>(obj.at(NAMEOF_MEMBER(&JsonCanChannelConfig::message_configs).c_str()))
    };
}

static void tag_invoke(json::value_from_tag, json::value& jv, JsonCanbusConfig const& config) {
    jv = {
        {NAMEOF_MEMBER(&JsonCanbusConfig::channel_configs), json::value_from(config.channel_configs)}
    };
}

static JsonCanbusConfig tag_invoke(json::value_to_tag<JsonCanbusConfig>, json::value const& jv) {
    json::object const& obj = jv.as_object();
    return JsonCanbusConfig{
        json::value_to<std::vector<JsonCanChannelConfig>>(obj.at(NAMEOF_MEMBER(&JsonCanbusConfig::channel_configs).c_str()))
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
