/*
 * Generated using zcbor version 0.9.1
 * https://github.com/NordicSemiconductor/zcbor
 * Generated with a --default-max-qty of 24
 */

#ifndef LOGGING_CONFIG_H__
#define LOGGING_CONFIG_H__

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>


#ifdef __cplusplus
extern "C" {
#endif

/** Which value for --default-max-qty this file was created with.
 *
 *  The define is used in the other generated file to do a build-time
 *  compatibility check.
 *
 *  See `zcbor --help` for more information about --default-max-qty
 */
#define DEFAULT_MAX_QTY 24

struct SensorLoggingConfig {
	bool is_enabled;
	bool log_raw_value;
	bool log_only_new_data;
};

struct SensorLoggingConfigMap_SensorLoggingConfig_m {
	uint32_t SensorLoggingConfig_m_key;
	struct SensorLoggingConfig SensorLoggingConfig_m;
};

struct SensorLoggingConfigMap {
	struct SensorLoggingConfigMap_SensorLoggingConfig_m SensorLoggingConfig_m[24];
	size_t SensorLoggingConfig_m_count;
};

struct LoggingConfig {
	uint32_t logging_interval_ms;
	uint32_t max_log_size_mb;
	struct SensorLoggingConfigMap sensor_configurations;
};

#ifdef __cplusplus
}
#endif

#endif /* LOGGING_CONFIG_H__ */
