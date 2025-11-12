#pragma once

#include <zephyr/data/json.h>

namespace eerie_leap::configuration::json::configs {

struct JsonComUserConfig {
    uint64_t device_id;
    uint32_t server_id;
};

struct JsonCanbusConfig {
    uint32_t bus_channel;
    uint32_t bitrate;
};

struct JsonSystemConfig {
    uint32_t com_user_refresh_rate_ms;
    JsonComUserConfig com_user_configs[8];
    size_t com_user_configs_len;
    JsonCanbusConfig canbus_configs[8];
    size_t canbus_configs_len;
};

static json_obj_descr json_com_user_config_descr[] = {
    JSON_OBJ_DESCR_PRIM(JsonComUserConfig, device_id, JSON_TOK_UINT64),
    JSON_OBJ_DESCR_PRIM(JsonComUserConfig, server_id, JSON_TOK_UINT),
};

static json_obj_descr json_canbus_config_descr[] = {
    JSON_OBJ_DESCR_PRIM(JsonCanbusConfig, bus_channel, JSON_TOK_UINT),
    JSON_OBJ_DESCR_PRIM(JsonCanbusConfig, bitrate, JSON_TOK_UINT),
};

static json_obj_descr json_system_config_descr[] = {
    JSON_OBJ_DESCR_PRIM(JsonSystemConfig, com_user_refresh_rate_ms, JSON_TOK_UINT),
    JSON_OBJ_DESCR_OBJ_ARRAY(JsonSystemConfig, com_user_configs, 8, com_user_configs_len, json_com_user_config_descr, ARRAY_SIZE(json_com_user_config_descr)),
    JSON_OBJ_DESCR_OBJ_ARRAY(JsonSystemConfig, canbus_configs, 8, canbus_configs_len, json_canbus_config_descr, ARRAY_SIZE(json_canbus_config_descr)),
};

} // namespace eerie_leap::configuration::json::configs
