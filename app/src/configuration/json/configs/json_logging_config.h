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
    std::vector<JsonSensorLoggingConfig> sensor_configurations;
};

static void tag_invoke(json::value_from_tag, json::value& jv, JsonSensorLoggingConfig const& config) {
    jv = {
        {NAMEOF_MEMBER(&JsonSensorLoggingConfig::sensor_id_hash), config.sensor_id_hash},
        {NAMEOF_MEMBER(&JsonSensorLoggingConfig::is_enabled), config.is_enabled},
        {NAMEOF_MEMBER(&JsonSensorLoggingConfig::log_raw_value), config.log_raw_value},
        {NAMEOF_MEMBER(&JsonSensorLoggingConfig::log_only_new_data), config.log_only_new_data}
    };
}

static JsonSensorLoggingConfig tag_invoke(json::value_to_tag<JsonSensorLoggingConfig>, json::value const& jv) {
    json::object const& obj = jv.as_object();
    return JsonSensorLoggingConfig{
        .sensor_id_hash = json::value_to<uint32_t>(obj.at(NAMEOF_MEMBER(&JsonSensorLoggingConfig::sensor_id_hash).c_str())),
        .is_enabled = json::value_to<bool>(obj.at(NAMEOF_MEMBER(&JsonSensorLoggingConfig::is_enabled).c_str())),
        .log_raw_value = json::value_to<bool>(obj.at(NAMEOF_MEMBER(&JsonSensorLoggingConfig::log_raw_value).c_str())),
        .log_only_new_data = json::value_to<bool>(obj.at(NAMEOF_MEMBER(&JsonSensorLoggingConfig::log_only_new_data).c_str()))
    };
}

static void tag_invoke(json::value_from_tag, json::value& jv, JsonLoggingConfig const& config) {
    jv = {
        {NAMEOF_MEMBER(&JsonLoggingConfig::logging_interval_ms), config.logging_interval_ms},
        {NAMEOF_MEMBER(&JsonLoggingConfig::max_log_size_mb), config.max_log_size_mb},
        {NAMEOF_MEMBER(&JsonLoggingConfig::sensor_configurations), json::value_from(config.sensor_configurations)}
    };
}

static JsonLoggingConfig tag_invoke(json::value_to_tag<JsonLoggingConfig>, json::value const& jv) {
    json::object const& obj = jv.as_object();
    return JsonLoggingConfig{
        .logging_interval_ms = json::value_to<uint32_t>(obj.at(NAMEOF_MEMBER(&JsonLoggingConfig::logging_interval_ms).c_str())),
        .max_log_size_mb = json::value_to<uint32_t>(obj.at(NAMEOF_MEMBER(&JsonLoggingConfig::max_log_size_mb).c_str())),
        .sensor_configurations = json::value_to<std::vector<JsonSensorLoggingConfig>>(obj.at(NAMEOF_MEMBER(&JsonLoggingConfig::sensor_configurations).c_str()))
    };
}

static std::unique_ptr<ExtString> json_encode_JsonLoggingConfig(const JsonLoggingConfig& config) {
    static BoostMemoryResource heap_mem_resource;
    json::storage_ptr sp = &heap_mem_resource;

    json::value jv = json::value_from(config, sp);

    ExtString result;
    result = json::serialize(jv);

    return std::make_unique<ExtString>(result);
}

static JsonLoggingConfig json_decode_JsonLoggingConfig(std::string_view json_str) {
    static BoostMemoryResource mem_resource;
    json::value jv = json::parse(json_str, &mem_resource);

    return json::value_to<JsonLoggingConfig>(jv);
}

} // namespace eerie_leap::configuration::json::configs
