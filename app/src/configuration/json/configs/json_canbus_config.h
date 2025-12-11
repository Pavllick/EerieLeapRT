#pragma once

#include <boost/container/pmr/vector.hpp>
#include <boost/json.hpp>
#include <nameof.hpp>

#include "utilities/memory/memory_resource_manager.h"

namespace eerie_leap::configuration::json::configs {

namespace json = boost::json;
using namespace eerie_leap::utilities::memory;

struct JsonCanSignalConfig {
    uint32_t start_bit;
    uint32_t size_bits;
    float factor;
    float offset;
    json::string name;
    json::string unit;

    JsonCanSignalConfig(json::storage_ptr sp = Mrm::GetBoostExtPmr())
        : name(sp), unit(sp) {}
};

struct JsonCanMessageConfig {
    uint32_t frame_id;
    uint32_t send_interval_ms;
    json::string script_path;

    json::string name;
	uint32_t message_size;
    boost::container::pmr::vector<JsonCanSignalConfig> signal_configs;

    JsonCanMessageConfig(json::storage_ptr sp = Mrm::GetBoostExtPmr())
        : script_path(sp), name(sp), signal_configs(sp.get()) {}
};

struct JsonCanChannelConfig {
    json::string type;
    bool is_extended_id;
    uint32_t bus_channel;
    uint32_t bitrate;
    uint32_t data_bitrate;
    json::string dbc_file_path;
    boost::container::pmr::vector<JsonCanMessageConfig> message_configs;

    JsonCanChannelConfig(json::storage_ptr sp = Mrm::GetBoostExtPmr())
        : type(sp), dbc_file_path(sp), message_configs(sp.get()) {}
};

struct JsonCanbusConfig {
    boost::container::pmr::vector<JsonCanChannelConfig> channel_configs;

    JsonCanbusConfig(json::storage_ptr sp = Mrm::GetBoostExtPmr())
        : channel_configs(sp.get()) {}
};

static void tag_invoke(json::value_from_tag, json::value& jv, JsonCanSignalConfig const& config) {
    jv.~value();
    new(&jv) json::value(json::object(Mrm::GetBoostExtPmr()));
    json::object& obj = jv.as_object();

    obj[NAMEOF_MEMBER(&JsonCanSignalConfig::start_bit).c_str()] = config.start_bit;
    obj[NAMEOF_MEMBER(&JsonCanSignalConfig::size_bits).c_str()] = config.size_bits;
    obj[NAMEOF_MEMBER(&JsonCanSignalConfig::factor).c_str()] = config.factor;
    obj[NAMEOF_MEMBER(&JsonCanSignalConfig::offset).c_str()] = config.offset;
    obj[NAMEOF_MEMBER(&JsonCanSignalConfig::name).c_str()] = config.name;
    obj[NAMEOF_MEMBER(&JsonCanSignalConfig::unit).c_str()] = config.unit;

    jv = std::move(obj);
}

static JsonCanSignalConfig tag_invoke(json::value_to_tag<JsonCanSignalConfig>, json::value const& jv) {
    json::object const& obj = jv.as_object();
    JsonCanSignalConfig result;

    result.start_bit = static_cast<uint32_t>(obj.at(NAMEOF_MEMBER(&JsonCanSignalConfig::start_bit).c_str()).as_int64());
    result.size_bits = static_cast<uint32_t>(obj.at(NAMEOF_MEMBER(&JsonCanSignalConfig::size_bits).c_str()).as_int64());
    result.factor = static_cast<float>(obj.at(NAMEOF_MEMBER(&JsonCanSignalConfig::factor).c_str()).to_number<double>());
    result.offset = static_cast<float>(obj.at(NAMEOF_MEMBER(&JsonCanSignalConfig::offset).c_str()).to_number<double>());
    result.name = obj.at(NAMEOF_MEMBER(&JsonCanSignalConfig::name).c_str()).as_string();
    result.unit = obj.at(NAMEOF_MEMBER(&JsonCanSignalConfig::unit).c_str()).as_string();

    return result;
}

static void tag_invoke(json::value_from_tag, json::value& jv, JsonCanMessageConfig const& config) {
    jv.~value();
    new(&jv) json::value(json::object(Mrm::GetBoostExtPmr()));
    json::object& obj = jv.as_object();

    obj[NAMEOF_MEMBER(&JsonCanMessageConfig::frame_id).c_str()] = config.frame_id;
    obj[NAMEOF_MEMBER(&JsonCanMessageConfig::send_interval_ms).c_str()] = config.send_interval_ms;
    obj[NAMEOF_MEMBER(&JsonCanMessageConfig::script_path).c_str()] = config.script_path;
    obj[NAMEOF_MEMBER(&JsonCanMessageConfig::name).c_str()] = config.name;
    obj[NAMEOF_MEMBER(&JsonCanMessageConfig::message_size).c_str()] = config.message_size;

    json::array signal_configs_array(Mrm::GetBoostExtPmr());
    for(const auto& signal_config : config.signal_configs)
        signal_configs_array.push_back(json::value_from(signal_config, Mrm::GetBoostExtPmr()));
    obj[NAMEOF_MEMBER(&JsonCanMessageConfig::signal_configs).c_str()] = std::move(signal_configs_array);

    jv = std::move(obj);
}

static JsonCanMessageConfig tag_invoke(json::value_to_tag<JsonCanMessageConfig>, json::value const& jv) {
    json::object const& obj = jv.as_object();
    JsonCanMessageConfig result;

    result.frame_id = static_cast<uint32_t>(obj.at(NAMEOF_MEMBER(&JsonCanMessageConfig::frame_id).c_str()).as_int64());
    result.send_interval_ms = static_cast<uint32_t>(obj.at(NAMEOF_MEMBER(&JsonCanMessageConfig::send_interval_ms).c_str()).as_int64());
    result.script_path = obj.at(NAMEOF_MEMBER(&JsonCanMessageConfig::script_path).c_str()).as_string();
    result.name = obj.at(NAMEOF_MEMBER(&JsonCanMessageConfig::name).c_str()).as_string();
    result.message_size = static_cast<uint32_t>(obj.at(NAMEOF_MEMBER(&JsonCanMessageConfig::message_size).c_str()).as_int64());

    const json::array& signal_configs_array = obj.at(NAMEOF_MEMBER(&JsonCanMessageConfig::signal_configs).c_str()).as_array();
    result.signal_configs.reserve(signal_configs_array.size());
    for(const auto& elem : signal_configs_array)
        result.signal_configs.push_back(json::value_to<JsonCanSignalConfig>(elem));

    return result;
}

static void tag_invoke(json::value_from_tag, json::value& jv, JsonCanChannelConfig const& config) {
    jv.~value();
    new(&jv) json::value(json::object(Mrm::GetBoostExtPmr()));
    json::object& obj = jv.as_object();

    obj[NAMEOF_MEMBER(&JsonCanChannelConfig::type).c_str()] = config.type;
    obj[NAMEOF_MEMBER(&JsonCanChannelConfig::is_extended_id).c_str()] = config.is_extended_id;
    obj[NAMEOF_MEMBER(&JsonCanChannelConfig::bus_channel).c_str()] = config.bus_channel;
    obj[NAMEOF_MEMBER(&JsonCanChannelConfig::bitrate).c_str()] = config.bitrate;
    obj[NAMEOF_MEMBER(&JsonCanChannelConfig::data_bitrate).c_str()] = config.data_bitrate;
    obj[NAMEOF_MEMBER(&JsonCanChannelConfig::dbc_file_path).c_str()] = config.dbc_file_path;

    json::array message_configs_array(Mrm::GetBoostExtPmr());
    for(const auto& message_config : config.message_configs)
        message_configs_array.push_back(json::value_from(message_config, Mrm::GetBoostExtPmr()));
    obj[NAMEOF_MEMBER(&JsonCanChannelConfig::message_configs).c_str()] = std::move(message_configs_array);

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
    new(&jv) json::value(json::object(Mrm::GetBoostExtPmr()));
    json::object& obj = jv.as_object();

    json::array channel_configs_array(Mrm::GetBoostExtPmr());
    for(const auto& channel_config : config.channel_configs)
        channel_configs_array.push_back(json::value_from(channel_config, Mrm::GetBoostExtPmr()));
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
