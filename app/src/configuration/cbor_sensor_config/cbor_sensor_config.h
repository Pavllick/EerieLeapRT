/*
 * Generated using zcbor version 0.9.1
 * https://github.com/NordicSemiconductor/zcbor
 * Generated with a --default-max-qty of 24
 */

#ifndef CBOR_SENSOR_CONFIG_H__
#define CBOR_SENSOR_CONFIG_H__

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <zcbor_common.h>

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

struct CborSensorMetadataConfig {
	struct zcbor_string name;
	struct zcbor_string unit;
	struct zcbor_string description;
};

struct CborSensorCalibrationDataMap_float32float {
	float float32float_key;
	float float32float;
};

struct CborSensorCalibrationDataMap {
	struct CborSensorCalibrationDataMap_float32float float32float[50];
	size_t float32float_count;
};

struct CborSensorConfigurationConfig {
	uint32_t type;
	uint32_t sampling_rate_ms;
	uint32_t interpolation_method;
	uint32_t channel;
	bool channel_present;
	struct zcbor_string connection_string;
	struct CborSensorCalibrationDataMap calibration_table;
	bool calibration_table_present;
	struct zcbor_string expression;
	bool expression_present;
};

struct CborSensorConfig {
	struct zcbor_string id;
	struct CborSensorMetadataConfig metadata;
	struct CborSensorConfigurationConfig configuration;
};

struct CborSensorsConfig {
	struct CborSensorConfig CborSensorConfig_m[24];
	size_t CborSensorConfig_m_count;
};

#ifdef __cplusplus
}
#endif

#endif /* CBOR_SENSOR_CONFIG_H__ */
