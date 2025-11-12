#pragma once

#include <zephyr/data/json.h>

namespace eerie_leap::configuration::json::configs {

struct JsonSensorMetadataConfig {
    const char* name;
    const char* unit;
    const char* description;
};

struct JsonSensorCalibrationDataConfig {
    float voltage;
    float value;
};

struct JsonSensorConfigurationConfig {
    const char* type;
    int32_t channel;
    const char* connection_string;
    uint32_t sampling_rate_ms;
    const char* interpolation_method;
    JsonSensorCalibrationDataConfig calibration_table[50];
    size_t calibration_table_len;
    const char* expression;
};

struct JsonSensorConfig {
    const char* id;
    JsonSensorMetadataConfig metadata;
    JsonSensorConfigurationConfig configuration;
};

struct JsonSensorsConfig {
    JsonSensorConfig sensors[24];
    size_t sensors_len;
};

static json_obj_descr json_sensor_metadata_descr[] = {
    JSON_OBJ_DESCR_PRIM(JsonSensorMetadataConfig, name, JSON_TOK_STRING),
    JSON_OBJ_DESCR_PRIM(JsonSensorMetadataConfig, unit, JSON_TOK_STRING),
    JSON_OBJ_DESCR_PRIM(JsonSensorMetadataConfig, description, JSON_TOK_STRING),
};

static json_obj_descr json_sensor_calibration_data_descr[] = {
    JSON_OBJ_DESCR_PRIM(JsonSensorCalibrationDataConfig, voltage, JSON_TOK_FLOAT_FP),
    JSON_OBJ_DESCR_PRIM(JsonSensorCalibrationDataConfig, value, JSON_TOK_FLOAT_FP),
};

static json_obj_descr json_sensor_configuration_descr[] = {
    JSON_OBJ_DESCR_PRIM(JsonSensorConfigurationConfig, type, JSON_TOK_STRING),
    JSON_OBJ_DESCR_PRIM(JsonSensorConfigurationConfig, channel, JSON_TOK_INT),
    JSON_OBJ_DESCR_PRIM(JsonSensorConfigurationConfig, connection_string, JSON_TOK_STRING),
    JSON_OBJ_DESCR_PRIM(JsonSensorConfigurationConfig, sampling_rate_ms, JSON_TOK_UINT),
    JSON_OBJ_DESCR_PRIM(JsonSensorConfigurationConfig, interpolation_method, JSON_TOK_STRING),
    JSON_OBJ_DESCR_OBJ_ARRAY(JsonSensorConfigurationConfig, calibration_table, 50, calibration_table_len, json_sensor_calibration_data_descr, ARRAY_SIZE(json_sensor_calibration_data_descr)),
    JSON_OBJ_DESCR_PRIM(JsonSensorConfigurationConfig, expression, JSON_TOK_STRING),
};

static json_obj_descr json_sensor_descr[] = {
    JSON_OBJ_DESCR_PRIM(JsonSensorConfig, id, JSON_TOK_STRING),
    JSON_OBJ_DESCR_OBJECT(JsonSensorConfig, metadata, json_sensor_metadata_descr),
    JSON_OBJ_DESCR_OBJECT(JsonSensorConfig, configuration, json_sensor_configuration_descr),
};

static json_obj_descr json_sensors_descr[] = {
    JSON_OBJ_DESCR_OBJ_ARRAY(JsonSensorsConfig, sensors, 24, sensors_len, json_sensor_descr, ARRAY_SIZE(json_sensor_descr)),
};

} // namespace eerie_leap::configuration::json::configs
