#pragma once

#include <vector>
#include <string>
#include <boost/json.hpp>
#include <nameof.hpp>

#include "utilities/memory/boost_memory_resource.h"

namespace eerie_leap::configuration::json::configs {

namespace json = boost::json;
using namespace eerie_leap::utilities::memory;

struct JsonCanMessageConfig {
    uint32_t frame_id;
    uint32_t send_interval_ms;
    json::string script_path;

    JsonCanMessageConfig(json::storage_ptr sp = &ext_boost_mem_resource)
        : script_path(sp) {}
};

struct JsonCanChannelConfig {
    json::string type;
    bool is_extended_id;
    uint32_t bus_channel;
    uint32_t bitrate;
    uint32_t data_bitrate;
    json::string dbc_file_path;
    std::vector<JsonCanMessageConfig, HeapAllocator<JsonCanMessageConfig>> message_configs;

    JsonCanChannelConfig(json::storage_ptr sp = &ext_boost_mem_resource)
        : type(sp), dbc_file_path(sp) {}
};

struct JsonCanbusConfig {
    std::vector<JsonCanChannelConfig, HeapAllocator<JsonCanChannelConfig>> channel_configs;
};

static void tag_invoke(json::value_from_tag, json::value& jv, JsonCanMessageConfig const& config) {
    jv.~value();
    new(&jv) json::value(json::object(&ext_boost_mem_resource));
    json::object& obj = jv.as_object();

    obj[NAMEOF_MEMBER(&JsonCanMessageConfig::frame_id).c_str()] = config.frame_id;
    obj[NAMEOF_MEMBER(&JsonCanMessageConfig::send_interval_ms).c_str()] = config.send_interval_ms;
    obj[NAMEOF_MEMBER(&JsonCanMessageConfig::script_path).c_str()] = config.script_path;

    jv = std::move(obj);
}

static JsonCanMessageConfig tag_invoke(json::value_to_tag<JsonCanMessageConfig>, json::value const& jv) {
    json::object const& obj = jv.as_object();
    JsonCanMessageConfig result;

    result.frame_id = static_cast<uint32_t>(obj.at(NAMEOF_MEMBER(&JsonCanMessageConfig::frame_id).c_str()).as_int64());
    result.send_interval_ms = static_cast<uint32_t>(obj.at(NAMEOF_MEMBER(&JsonCanMessageConfig::send_interval_ms).c_str()).as_int64());
    result.script_path = obj.at(NAMEOF_MEMBER(&JsonCanMessageConfig::script_path).c_str()).as_string();

    return result;
}

static void tag_invoke(json::value_from_tag, json::value& jv, JsonCanChannelConfig const& config) {
    jv.~value();
    new(&jv) json::value(json::object(&ext_boost_mem_resource));
    json::object& obj = jv.as_object();

    obj[NAMEOF_MEMBER(&JsonCanChannelConfig::type).c_str()] = config.type;
    obj[NAMEOF_MEMBER(&JsonCanChannelConfig::is_extended_id).c_str()] = config.is_extended_id;
    obj[NAMEOF_MEMBER(&JsonCanChannelConfig::bus_channel).c_str()] = config.bus_channel;
    obj[NAMEOF_MEMBER(&JsonCanChannelConfig::bitrate).c_str()] = config.bitrate;
    obj[NAMEOF_MEMBER(&JsonCanChannelConfig::data_bitrate).c_str()] = config.data_bitrate;
    obj[NAMEOF_MEMBER(&JsonCanChannelConfig::dbc_file_path).c_str()] = config.dbc_file_path;
    obj[NAMEOF_MEMBER(&JsonCanChannelConfig::message_configs).c_str()] = json::value_from(config.message_configs);

    jv = std::move(obj);
}

static JsonCanChannelConfig tag_invoke(json::value_to_tag<JsonCanChannelConfig>, json::value const& jv) {
    json::object const& obj = jv.as_object();
    JsonCanChannelConfig result;

    result.type = obj.at(NAMEOF_MEMBER(&JsonCanChannelConfig::type).c_str()).as_string();
    result.is_extended_id = obj.at(NAMEOF_MEMBER(&JsonCanChannelConfig::is_extended_id).c_str()).as_bool();
    result.bus_channel = static_cast<uint32_t>(obj.at(NAMEOF_MEMBER(&JsonCanChannelConfig::bus_channel).c_str()).as_int64());
    result.bitrate = static_cast<uint32_t>(obj.at(NAMEOF_MEMBER(&JsonCanChannelConfig::bitrate).c_str()).as_int64());
    result.data_bitrate = static_cast<uint32_t>(obj.at(NAMEOF_MEMBER(&JsonCanChannelConfig::data_bitrate).c_str()).as_int64());
    result.dbc_file_path = obj.at(NAMEOF_MEMBER(&JsonCanChannelConfig::dbc_file_path).c_str()).as_string();

    const json::array& message_configs_array = obj.at(NAMEOF_MEMBER(&JsonCanChannelConfig::message_configs).c_str()).as_array();
    result.message_configs.reserve(message_configs_array.size());
    for(const auto& elem : message_configs_array)
        result.message_configs.push_back(json::value_to<JsonCanMessageConfig>(elem));

    return result;
}

static void tag_invoke(json::value_from_tag, json::value& jv, JsonCanbusConfig const& config) {
    jv.~value();
    new(&jv) json::value(json::object(&ext_boost_mem_resource));
    json::object& obj = jv.as_object();

    json::array channel_configs_array(&ext_boost_mem_resource);
    for(const auto& channel_config : config.channel_configs)
        channel_configs_array.push_back(json::value_from(channel_config, &ext_boost_mem_resource));
    obj[NAMEOF_MEMBER(&JsonCanbusConfig::channel_configs).c_str()] = std::move(channel_configs_array);

    jv = std::move(obj);
}

static JsonCanbusConfig tag_invoke(json::value_to_tag<JsonCanbusConfig>, json::value const& jv) {
    json::object const& obj = jv.as_object();
    JsonCanbusConfig result;

    const json::array& channel_configs_array = obj.at(NAMEOF_MEMBER(&JsonCanbusConfig::channel_configs).c_str()).as_array();
    result.channel_configs.reserve(channel_configs_array.size());
    for(const auto& elem : channel_configs_array)
        result.channel_configs.push_back(json::value_to<JsonCanChannelConfig>(elem));

    return result;
}

} // namespace eerie_leap::configuration::json::configs
