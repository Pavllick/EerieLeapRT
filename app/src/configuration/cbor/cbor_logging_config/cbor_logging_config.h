/*
 * Generated using zcbor version 0.9.1
 * https://github.com/NordicSemiconductor/zcbor
 * Generated with a --default-max-qty of 24
 */

#ifndef CBOR_LOGGING_CONFIG_H__
#define CBOR_LOGGING_CONFIG_H__

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

struct CborSensorLoggingConfig {
	uint32_t sensor_id_hash;
	bool is_enabled;
	bool log_raw_value;
	bool log_only_new_data;
};

struct CborLoggingConfig {
	uint32_t logging_interval_ms;
	uint32_t max_log_size_mb;
	struct CborSensorLoggingConfig CborSensorLoggingConfig_m[24];
	size_t CborSensorLoggingConfig_m_count;
};

#ifdef __cplusplus
}
#endif

#endif /* CBOR_LOGGING_CONFIG_H__ */
