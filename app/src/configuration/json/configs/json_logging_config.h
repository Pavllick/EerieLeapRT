#pragma once

#include <zephyr/data/json.h>

namespace eerie_leap::configuration::json::configs {

struct JsonSensorLoggingConfig {
    uint32_t sensor_id_hash;
    bool is_enabled;
    bool log_raw_value;
    bool log_only_new_data;
};

struct JsonLoggingConfig {
    uint32_t logging_interval_ms;
    uint32_t max_log_size_mb;
    JsonSensorLoggingConfig sensor_configurations[24];
    size_t sensor_configurations_len;
};

static json_obj_descr json_sensor_logging_config_descr[] = {
    JSON_OBJ_DESCR_PRIM(JsonSensorLoggingConfig, sensor_id_hash, JSON_TOK_UINT),
    JSON_OBJ_DESCR_PRIM(JsonSensorLoggingConfig, is_enabled, JSON_TOK_TRUE),
    JSON_OBJ_DESCR_PRIM(JsonSensorLoggingConfig, log_raw_value, JSON_TOK_TRUE),
    JSON_OBJ_DESCR_PRIM(JsonSensorLoggingConfig, log_only_new_data, JSON_TOK_TRUE),
};

static json_obj_descr json_logging_config_descr[] = {
    JSON_OBJ_DESCR_PRIM(JsonLoggingConfig, logging_interval_ms, JSON_TOK_UINT),
    JSON_OBJ_DESCR_PRIM(JsonLoggingConfig, max_log_size_mb, JSON_TOK_UINT),
    JSON_OBJ_DESCR_OBJ_ARRAY(JsonLoggingConfig, sensor_configurations, 24, sensor_configurations_len, json_sensor_logging_config_descr, ARRAY_SIZE(json_sensor_logging_config_descr)),
};

} // namespace eerie_leap::configuration::json::configs
