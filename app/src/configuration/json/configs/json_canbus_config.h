#pragma once

#include <zephyr/data/json.h>

namespace eerie_leap::configuration::json::configs {

struct JsonCanMessageConfig {
    uint32_t frame_id;
    uint32_t send_interval_ms;
};

struct JsonCanChannelConfig {
    const char* type;
    uint32_t bus_channel;
    uint32_t bitrate;
    JsonCanMessageConfig message_configs[24];
    size_t message_configs_len;
};

struct JsonCanbusConfig {
    JsonCanChannelConfig channel_configs[8];
    size_t channel_configs_len;
};

static json_obj_descr json_can_message_config_descr[] = {
    JSON_OBJ_DESCR_PRIM(JsonCanMessageConfig, frame_id, JSON_TOK_UINT),
    JSON_OBJ_DESCR_PRIM(JsonCanMessageConfig, send_interval_ms, JSON_TOK_UINT),
};

static json_obj_descr json_can_channel_config_descr[] = {
    JSON_OBJ_DESCR_PRIM(JsonCanChannelConfig, type, JSON_TOK_STRING),
    JSON_OBJ_DESCR_PRIM(JsonCanChannelConfig, bus_channel, JSON_TOK_UINT),
    JSON_OBJ_DESCR_PRIM(JsonCanChannelConfig, bitrate, JSON_TOK_UINT),
    JSON_OBJ_DESCR_OBJ_ARRAY(JsonCanChannelConfig, message_configs, 24, message_configs_len, json_can_message_config_descr, ARRAY_SIZE(json_can_message_config_descr)),
};

static json_obj_descr json_canbus_config_descr[] = {
    JSON_OBJ_DESCR_OBJ_ARRAY(JsonCanbusConfig, channel_configs, 8, channel_configs_len, json_can_channel_config_descr, ARRAY_SIZE(json_can_channel_config_descr)),
};

} // namespace eerie_leap::configuration::json::configs
