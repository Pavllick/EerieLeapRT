#pragma once

#include <vector>
#include <string>
#include <boost/json.hpp>
#include <nameof.hpp>

#include "utilities/memory/boost_memory_resource.h"

namespace eerie_leap::configuration::json::configs {

namespace json = boost::json;
using namespace eerie_leap::utilities::memory;

struct JsonComUserConfig {
    uint64_t device_id;
    uint32_t server_id;
};

struct JsonSystemConfig {
    uint32_t com_user_refresh_rate_ms;
    std::vector<JsonComUserConfig> com_user_configs;
};

static void tag_invoke(json::value_from_tag, json::value& jv, JsonComUserConfig const& config) {
    jv.~value();
    new(&jv) json::value(json::object(ext_boost_json_storage_ptr));
    json::object& obj = jv.as_object();

    obj[NAMEOF_MEMBER(&JsonComUserConfig::device_id).c_str()] = config.device_id;
    obj[NAMEOF_MEMBER(&JsonComUserConfig::server_id).c_str()] = config.server_id;

    jv = std::move(obj);
}

static JsonComUserConfig tag_invoke(json::value_to_tag<JsonComUserConfig>, json::value const& jv) {
    json::object const& obj = jv.as_object();
    return {
        .device_id = static_cast<uint64_t>(obj.at(NAMEOF_MEMBER(&JsonComUserConfig::device_id).c_str()).as_uint64()),
        .server_id = static_cast<uint32_t>(obj.at(NAMEOF_MEMBER(&JsonComUserConfig::server_id).c_str()).as_int64())
    };
}

static void tag_invoke(json::value_from_tag, json::value& jv, JsonSystemConfig const& config) {
    jv.~value();
    new(&jv) json::value(json::object(ext_boost_json_storage_ptr));
    json::object& obj = jv.as_object();

    obj[NAMEOF_MEMBER(&JsonSystemConfig::com_user_refresh_rate_ms).c_str()] = config.com_user_refresh_rate_ms;

    json::array com_user_configs_array(ext_boost_json_storage_ptr);
    for(const auto& com_user_config : config.com_user_configs)
        com_user_configs_array.push_back(json::value_from(com_user_config, ext_boost_json_storage_ptr));
    obj[NAMEOF_MEMBER(&JsonSystemConfig::com_user_configs).c_str()] = std::move(com_user_configs_array);

    jv = std::move(obj);
}

static JsonSystemConfig tag_invoke(json::value_to_tag<JsonSystemConfig>, json::value const& jv) {
    json::object const& obj = jv.as_object();
    JsonSystemConfig result;

    result.com_user_refresh_rate_ms = static_cast<uint32_t>(obj.at(NAMEOF_MEMBER(&JsonSystemConfig::com_user_refresh_rate_ms).c_str()).as_int64());

    const json::array& com_user_configs_array = obj.at(NAMEOF_MEMBER(&JsonSystemConfig::com_user_configs).c_str()).as_array();
    result.com_user_configs.reserve(com_user_configs_array.size());
    for(const auto& elem : com_user_configs_array)
        result.com_user_configs.push_back(json::value_to<JsonComUserConfig>(elem));

    return result;
}

} // namespace eerie_leap::configuration::json::configs
