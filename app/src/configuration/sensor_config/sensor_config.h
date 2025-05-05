/*
 * Generated using zcbor version 0.9.1
 * https://github.com/NordicSemiconductor/zcbor
 * Generated with a --default-max-qty of 24
 */

#ifndef SENSOR_CONFIG_H__
#define SENSOR_CONFIG_H__

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

struct SensorMetadataConfig {
	struct zcbor_string name;
	struct zcbor_string unit;
	struct zcbor_string description;
	bool description_present;
};

struct SensorCalibrationDataMap_floatfloat {
	double floatfloat_key;
	double floatfloat;
};

struct SensorCalibrationDataMap {
	struct SensorCalibrationDataMap_floatfloat floatfloat[50];
	size_t floatfloat_count;
};

struct SensorConfigurationConfig {
	uint32_t type;
	uint32_t sampling_rate_ms;
	uint32_t interpolation_method;
	uint32_t channel;
	bool channel_present;
	struct SensorCalibrationDataMap calibration_table;
	bool calibration_table_present;
	struct zcbor_string expression;
	bool expression_present;
};

struct SensorConfig {
	struct zcbor_string id;
	struct SensorMetadataConfig metadata;
	struct SensorConfigurationConfig configuration;
};

struct SensorsConfig {
	struct SensorConfig SensorConfig_m[24];
	size_t SensorConfig_m_count;
};

#ifdef __cplusplus
}
#endif

#endif /* SENSOR_CONFIG_H__ */
