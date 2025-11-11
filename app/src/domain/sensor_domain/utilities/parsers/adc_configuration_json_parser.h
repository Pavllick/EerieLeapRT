#pragma once

#include <span>

#include <zephyr/data/json.h>

#include "utilities/memory/heap_allocator.h"
#include "subsys/adc/models/adc_configuration.h"

namespace eerie_leap::domain::sensor_domain::utilities::parsers {

using namespace eerie_leap::utilities::memory;
using namespace eerie_leap::subsys::adc::models;

struct AdcCalibrationDataJsonDto {
    float voltage;
    float value;
};

struct AdcChannelConfigurationJsonDto {
    const char* interpolation_method;
    AdcCalibrationDataJsonDto calibration_table[50];
    size_t calibration_table_len;
};

struct AdcConfigurationJsonDto {
    uint32_t samples;
    AdcChannelConfigurationJsonDto channel_configurations[24];
    size_t channel_configurations_len;
};

static json_obj_descr adc_calibration_data_map_descr[] = {
    JSON_OBJ_DESCR_PRIM(AdcCalibrationDataJsonDto, voltage, JSON_TOK_FLOAT_FP),
    JSON_OBJ_DESCR_PRIM(AdcCalibrationDataJsonDto, value, JSON_TOK_FLOAT_FP),
};

static json_obj_descr adc_channel_configuration_descr[] = {
    JSON_OBJ_DESCR_PRIM(AdcChannelConfigurationJsonDto, interpolation_method, JSON_TOK_STRING),
    JSON_OBJ_DESCR_OBJ_ARRAY(AdcChannelConfigurationJsonDto, calibration_table, 50, calibration_table_len, adc_calibration_data_map_descr, ARRAY_SIZE(adc_calibration_data_map_descr)),
};

static json_obj_descr adc_configuration_descr[] = {
    JSON_OBJ_DESCR_PRIM(AdcConfigurationJsonDto, samples, JSON_TOK_UINT),
    JSON_OBJ_DESCR_OBJ_ARRAY(AdcConfigurationJsonDto, channel_configurations, 24, channel_configurations_len, adc_channel_configuration_descr, ARRAY_SIZE(adc_channel_configuration_descr)),
};

class AdcConfigurationJsonParser {
public:
    AdcConfigurationJsonParser() = default;

    ext_unique_ptr<ExtVector> Serialize(const AdcConfiguration& adc_configuration);
    AdcConfiguration Deserialize(const std::span<const uint8_t>& json);
};

} // namespace eerie_leap::domain::sensor_domain::utilities::parsers
