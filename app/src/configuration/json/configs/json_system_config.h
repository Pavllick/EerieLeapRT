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
    jv = {
        {NAMEOF_MEMBER(&JsonComUserConfig::device_id), config.device_id},
        {NAMEOF_MEMBER(&JsonComUserConfig::server_id), config.server_id}
    };
}

static JsonComUserConfig tag_invoke(json::value_to_tag<JsonComUserConfig>, json::value const& jv) {
    json::object const& obj = jv.as_object();
    return JsonComUserConfig{
        .device_id = json::value_to<uint64_t>(obj.at(NAMEOF_MEMBER(&JsonComUserConfig::device_id).c_str())),
        .server_id = json::value_to<uint32_t>(obj.at(NAMEOF_MEMBER(&JsonComUserConfig::server_id).c_str()))
    };
}

static void tag_invoke(json::value_from_tag, json::value& jv, JsonSystemConfig const& config) {
    jv = {
        {NAMEOF_MEMBER(&JsonSystemConfig::com_user_refresh_rate_ms), config.com_user_refresh_rate_ms},
        {NAMEOF_MEMBER(&JsonSystemConfig::com_user_configs), json::value_from(config.com_user_configs)}
    };
}

static JsonSystemConfig tag_invoke(json::value_to_tag<JsonSystemConfig>, json::value const& jv) {
    json::object const& obj = jv.as_object();
    return JsonSystemConfig{
        .com_user_refresh_rate_ms = json::value_to<uint32_t>(obj.at(NAMEOF_MEMBER(&JsonSystemConfig::com_user_refresh_rate_ms).c_str())),
        .com_user_configs = json::value_to<std::vector<JsonComUserConfig>>(obj.at(NAMEOF_MEMBER(&JsonSystemConfig::com_user_configs).c_str()))
    };
}

static std::unique_ptr<ExtString> json_encode_JsonSystemConfig(const JsonSystemConfig& config) {
    json::value jv = json::value_from(config, &ext_boost_mem_resource);

    ExtString result;
    result = json::serialize(jv);

    return std::make_unique<ExtString>(result);
}

static JsonSystemConfig json_decode_JsonSystemConfig(std::string_view json_str) {
    json::value jv = json::parse(json_str, &ext_boost_mem_resource);

    return json::value_to<JsonSystemConfig>(jv);
}

} // namespace eerie_leap::configuration::json::configs
