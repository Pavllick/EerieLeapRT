#pragma once

#include <vector>
#include <string>
#include <boost/json.hpp>
#include <nameof.hpp>

#include "utilities/memory/boost_memory_resource.h"

namespace eerie_leap::configuration::json::configs {

namespace json = boost::json;
using namespace eerie_leap::utilities::memory;

struct JsonSensorLoggingConfig {
    uint32_t sensor_id_hash;
    bool is_enabled;
    bool log_raw_value;
    bool log_only_new_data;
};

struct JsonLoggingConfig {
    uint32_t logging_interval_ms;
    uint32_t max_log_size_mb;
    std::vector<JsonSensorLoggingConfig, HeapAllocator<JsonSensorLoggingConfig>> sensor_configs;
};

static void tag_invoke(json::value_from_tag, json::value& jv, JsonSensorLoggingConfig const& config) {
    jv.~value();
    new(&jv) json::value(json::object(&ext_boost_mem_resource));
    json::object& obj = jv.as_object();

    obj[NAMEOF_MEMBER(&JsonSensorLoggingConfig::sensor_id_hash).c_str()] = config.sensor_id_hash;
    obj[NAMEOF_MEMBER(&JsonSensorLoggingConfig::is_enabled).c_str()] = config.is_enabled;
    obj[NAMEOF_MEMBER(&JsonSensorLoggingConfig::log_raw_value).c_str()] = config.log_raw_value;
    obj[NAMEOF_MEMBER(&JsonSensorLoggingConfig::log_only_new_data).c_str()] = config.log_only_new_data;

    jv = std::move(obj);
}

static JsonSensorLoggingConfig tag_invoke(json::value_to_tag<JsonSensorLoggingConfig>, json::value const& jv) {
    json::object const& obj = jv.as_object();
    return {
        .sensor_id_hash = static_cast<uint32_t>(obj.at(NAMEOF_MEMBER(&JsonSensorLoggingConfig::sensor_id_hash).c_str()).as_int64()),
        .is_enabled = obj.at(NAMEOF_MEMBER(&JsonSensorLoggingConfig::is_enabled).c_str()).as_bool(),
        .log_raw_value = obj.at(NAMEOF_MEMBER(&JsonSensorLoggingConfig::log_raw_value).c_str()).as_bool(),
        .log_only_new_data = obj.at(NAMEOF_MEMBER(&JsonSensorLoggingConfig::log_only_new_data).c_str()).as_bool()
    };
}

static void tag_invoke(json::value_from_tag, json::value& jv, JsonLoggingConfig const& config) {
    jv.~value();
    new(&jv) json::value(json::object(&ext_boost_mem_resource));
    json::object& obj = jv.as_object();

    obj[NAMEOF_MEMBER(&JsonLoggingConfig::logging_interval_ms).c_str()] = config.logging_interval_ms;
    obj[NAMEOF_MEMBER(&JsonLoggingConfig::max_log_size_mb).c_str()] = config.max_log_size_mb;

    json::array sensor_configs_array(&ext_boost_mem_resource);
    for(const auto& sensor_config : config.sensor_configs)
        sensor_configs_array.push_back(json::value_from(sensor_config, &ext_boost_mem_resource));
    obj[NAMEOF_MEMBER(&JsonLoggingConfig::sensor_configs).c_str()] = std::move(sensor_configs_array);

    jv = std::move(obj);
}

static JsonLoggingConfig tag_invoke(json::value_to_tag<JsonLoggingConfig>, json::value const& jv) {
    json::object const& obj = jv.as_object();
    JsonLoggingConfig result;

    result.logging_interval_ms = static_cast<uint32_t>(obj.at(NAMEOF_MEMBER(&JsonLoggingConfig::logging_interval_ms).c_str()).as_int64());
    result.max_log_size_mb = static_cast<uint32_t>(obj.at(NAMEOF_MEMBER(&JsonLoggingConfig::max_log_size_mb).c_str()).as_int64());

    const json::array& sensor_configs_array = obj.at(NAMEOF_MEMBER(&JsonLoggingConfig::sensor_configs).c_str()).as_array();
    result.sensor_configs.reserve(sensor_configs_array.size());
    for(const auto& elem : sensor_configs_array)
        result.sensor_configs.push_back(json::value_to<JsonSensorLoggingConfig>(elem));

    return result;
}

} // namespace eerie_leap::configuration::json::configs
