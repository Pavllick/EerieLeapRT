/*
 * Generated using zcbor version 0.9.1
 * https://github.com/NordicSemiconductor/zcbor
 * Generated with a --default-max-qty of 24
 */

#ifndef ADC_CONFIG_H__
#define ADC_CONFIG_H__

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

struct AdcCalibrationDataMap_float32float {
	float float32float_key;
	float float32float;
};

struct AdcCalibrationDataMap {
	struct AdcCalibrationDataMap_float32float float32float[50];
	size_t float32float_count;
};

struct AdcChannelConfig {
	uint32_t interpolation_method;
	struct AdcCalibrationDataMap calibration_table;
	bool calibration_table_present;
};

struct AdcConfig {
	uint32_t samples;
	struct AdcChannelConfig AdcChannelConfig_m[24];
	size_t AdcChannelConfig_m_count;
};

#ifdef __cplusplus
}
#endif

#endif /* ADC_CONFIG_H__ */
