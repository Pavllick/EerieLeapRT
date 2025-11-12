#pragma once

#include <zephyr/data/json.h>

namespace eerie_leap::configuration::json::configs {

struct JsonAdcCalibrationDataConfig {
    float voltage;
    float value;
};

struct JsonAdcChannelConfig {
    const char* interpolation_method;
    JsonAdcCalibrationDataConfig calibration_table[50];
    size_t calibration_table_len;
};

struct JsonAdcConfig {
    uint32_t samples;
    JsonAdcChannelConfig channel_configurations[24];
    size_t channel_configurations_len;
};

static json_obj_descr json_adc_calibration_data_config_descr[] = {
    JSON_OBJ_DESCR_PRIM(JsonAdcCalibrationDataConfig, voltage, JSON_TOK_FLOAT_FP),
    JSON_OBJ_DESCR_PRIM(JsonAdcCalibrationDataConfig, value, JSON_TOK_FLOAT_FP),
};

static json_obj_descr json_adc_channel_config_descr[] = {
    JSON_OBJ_DESCR_PRIM(JsonAdcChannelConfig, interpolation_method, JSON_TOK_STRING),
    JSON_OBJ_DESCR_OBJ_ARRAY(JsonAdcChannelConfig, calibration_table, 50, calibration_table_len, json_adc_calibration_data_config_descr, ARRAY_SIZE(json_adc_calibration_data_config_descr)),
};

static json_obj_descr json_adc_config_descr[] = {
    JSON_OBJ_DESCR_PRIM(JsonAdcConfig, samples, JSON_TOK_UINT),
    JSON_OBJ_DESCR_OBJ_ARRAY(JsonAdcConfig, channel_configurations, 24, channel_configurations_len, json_adc_channel_config_descr, ARRAY_SIZE(json_adc_channel_config_descr)),
};

} // namespace eerie_leap::configuration::json::configs
