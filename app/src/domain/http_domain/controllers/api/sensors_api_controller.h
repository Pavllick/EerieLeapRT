#pragma once

#include <memory>

#include <zephyr/net/http/service.h>
#include <zephyr/data/json.h>

#include "controllers/sensors_configuration_controller.h"
#include "domain/sensor_domain/services/processing_scheduler_serivce.h"

namespace eerie_leap::domain::http_domain::controllers::api {

using namespace eerie_leap::controllers;
using namespace eerie_leap::domain::sensor_domain::services;

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

class SensorsApiController {
private:
    static const size_t sensors_config_post_buffer_size_ = 24576;

    static std::shared_ptr<ExtVector> sensors_config_post_buffer_;
    static std::shared_ptr<ExtVector> sensors_config_get_buffer_;

    static std::shared_ptr<MathParserService> math_parser_service_;
    static std::shared_ptr<SensorsConfigurationController> sensors_configuration_controller_;
    static std::shared_ptr<ProcessingSchedulerService> processing_scheduler_service_;

    static void ParseSensorsConfigJson(uint8_t *buffer, size_t len);

    static int sensors_config_get_handler(http_client_ctx *client, enum http_data_status status, const http_request_ctx *request_ctx, http_response_ctx *response_ctx, void *user_data);
    static int sensors_config_post_handler(http_client_ctx *client, enum http_data_status status, const http_request_ctx *request_ctx, http_response_ctx *response_ctx, void *user_data);
    static int sensors_config_handler(http_client_ctx *client, enum http_data_status status, const http_request_ctx *request_ctx, http_response_ctx *response_ctx, void *user_data);

public:
    static http_resource_detail_dynamic sensors_config_resource_detail;

    SensorsApiController(
        std::shared_ptr<MathParserService> math_parser_service,
        std::shared_ptr<SensorsConfigurationController> sensors_configuration_controller,
        std::shared_ptr<ProcessingSchedulerService> processing_scheduler_service);
};

} // namespace eerie_leap::domain::http_domain::controllers::api
