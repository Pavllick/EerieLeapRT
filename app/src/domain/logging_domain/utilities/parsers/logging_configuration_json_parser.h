#pragma once

#include <span>

#include <zephyr/data/json.h>

#include "utilities/memory/heap_allocator.h"
#include "domain/logging_domain/models/logging_configuration.h"

namespace eerie_leap::domain::logging_domain::utilities::parsers {

using namespace eerie_leap::utilities::memory;
using namespace eerie_leap::domain::logging_domain::models;

struct SensorLoggingConfigurationJsonDto {
    uint32_t sensor_id_hash;
    bool is_enabled;
    bool log_raw_value;
    bool log_only_new_data;
};

struct LoggingConfigurationJsonDto {
    uint32_t logging_interval_ms;
    uint32_t max_log_size_mb;
    SensorLoggingConfigurationJsonDto sensor_configurations[24];
    size_t sensor_configurations_len;
};

static json_obj_descr sensor_logging_configuration_descr[] = {
    JSON_OBJ_DESCR_PRIM(SensorLoggingConfigurationJsonDto, sensor_id_hash, JSON_TOK_UINT),
    JSON_OBJ_DESCR_PRIM(SensorLoggingConfigurationJsonDto, is_enabled, JSON_TOK_TRUE),
    JSON_OBJ_DESCR_PRIM(SensorLoggingConfigurationJsonDto, log_raw_value, JSON_TOK_TRUE),
    JSON_OBJ_DESCR_PRIM(SensorLoggingConfigurationJsonDto, log_only_new_data, JSON_TOK_TRUE),
};

static json_obj_descr logging_configuration_descr[] = {
    JSON_OBJ_DESCR_PRIM(LoggingConfigurationJsonDto, logging_interval_ms, JSON_TOK_UINT),
    JSON_OBJ_DESCR_PRIM(LoggingConfigurationJsonDto, max_log_size_mb, JSON_TOK_UINT),
    JSON_OBJ_DESCR_OBJ_ARRAY(LoggingConfigurationJsonDto, sensor_configurations, 24, sensor_configurations_len, sensor_logging_configuration_descr, ARRAY_SIZE(sensor_logging_configuration_descr)),
};

class LoggingConfigurationJsonParser {
public:
    LoggingConfigurationJsonParser() = default;

    ext_unique_ptr<ExtVector> Serialize(const LoggingConfiguration& logging_configuration);
    LoggingConfiguration Deserialize(const std::span<const uint8_t>& json);
};

} // namespace eerie_leap::domain::logging_domain::utilities::parsers
