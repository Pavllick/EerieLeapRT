#pragma once

#include <memory>
#include <span>

#include <zephyr/data/json.h>

#include "utilities/memory/heap_allocator.h"
#include "utilities/math_parser/math_parser_service.hpp"
#include "domain/sensor_domain/models/sensor.h"

namespace eerie_leap::domain::sensor_domain::utilities::parsers {

using namespace eerie_leap::utilities::memory;
using namespace eerie_leap::utilities::math_parser;
using namespace eerie_leap::domain::sensor_domain::models;

struct SensorMetadataJsonDto {
    const char* name;
    const char* unit;
    const char* description;
};

struct SensorCalibrationDataMapJsonDto {
    float voltage;
    float value;
};

struct SensorConfigurationJsonDto {
    const char* type;
    int32_t channel;
    const char* connection_string;
    uint32_t sampling_rate_ms;
    const char* interpolation_method;
    SensorCalibrationDataMapJsonDto calibration_table[50];
    size_t calibration_table_len;
    const char* expression;
};

struct SensorJsonDto {
    const char* id;
    SensorMetadataJsonDto metadata;
    SensorConfigurationJsonDto configuration;
};

struct SensorsJsonDto {
    SensorJsonDto sensors[24];
    size_t sensors_len;
};

static json_obj_descr sensor_metadata_descr[] = {
    JSON_OBJ_DESCR_PRIM(SensorMetadataJsonDto, name, JSON_TOK_STRING),
    JSON_OBJ_DESCR_PRIM(SensorMetadataJsonDto, unit, JSON_TOK_STRING),
    JSON_OBJ_DESCR_PRIM(SensorMetadataJsonDto, description, JSON_TOK_STRING),
};

static json_obj_descr sensor_calibration_data_map_descr[] = {
    JSON_OBJ_DESCR_PRIM(SensorCalibrationDataMapJsonDto, voltage, JSON_TOK_FLOAT_FP),
    JSON_OBJ_DESCR_PRIM(SensorCalibrationDataMapJsonDto, value, JSON_TOK_FLOAT_FP),
};

static json_obj_descr sensor_configuration_descr[] = {
    JSON_OBJ_DESCR_PRIM(SensorConfigurationJsonDto, type, JSON_TOK_STRING),
    JSON_OBJ_DESCR_PRIM(SensorConfigurationJsonDto, channel, JSON_TOK_INT),
    JSON_OBJ_DESCR_PRIM(SensorConfigurationJsonDto, connection_string, JSON_TOK_STRING),
    JSON_OBJ_DESCR_PRIM(SensorConfigurationJsonDto, sampling_rate_ms, JSON_TOK_UINT),
    JSON_OBJ_DESCR_PRIM(SensorConfigurationJsonDto, interpolation_method, JSON_TOK_STRING),
    JSON_OBJ_DESCR_OBJ_ARRAY(SensorConfigurationJsonDto, calibration_table, 50, calibration_table_len, sensor_calibration_data_map_descr, ARRAY_SIZE(sensor_calibration_data_map_descr)),
    JSON_OBJ_DESCR_PRIM(SensorConfigurationJsonDto, expression, JSON_TOK_STRING),
};

static json_obj_descr sensor_descr[] = {
    JSON_OBJ_DESCR_PRIM(SensorJsonDto, id, JSON_TOK_STRING),
    JSON_OBJ_DESCR_OBJECT(SensorJsonDto, metadata, sensor_metadata_descr),
    JSON_OBJ_DESCR_OBJECT(SensorJsonDto, configuration, sensor_configuration_descr),
};

static json_obj_descr sensors_descr[] = {
    JSON_OBJ_DESCR_OBJ_ARRAY(SensorsJsonDto, sensors, 24, sensors_len, sensor_descr, ARRAY_SIZE(sensor_descr)),
};

class SensorsJsonParser {
private:
    std::shared_ptr<MathParserService> math_parser_service_;

public:
    explicit SensorsJsonParser(std::shared_ptr<MathParserService> math_parser_service);

    ext_unique_ptr<ExtVector> Serialize(
        const std::vector<std::shared_ptr<Sensor>>& sensors,
        uint32_t gpio_channel_count,
        uint32_t adc_channel_count);
    std::vector<std::shared_ptr<Sensor>> Deserialize(
        const std::span<const uint8_t>& json,
        uint32_t gpio_channel_count,
        uint32_t adc_channel_count);
};

} // namespace eerie_leap::domain::sensor_domain::utilities::parsers
