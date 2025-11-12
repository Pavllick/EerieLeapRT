/*
 * Generated using zcbor version 0.9.1
 * https://github.com/NordicSemiconductor/zcbor
 * Generated with a --default-max-qty of 24
 */

#ifndef CBOR_ADC_CONFIG_H__
#define CBOR_ADC_CONFIG_H__

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

struct CborAdcCalibrationDataMap_float32float {
	float float32float_key;
	float float32float;
};

struct CborAdcCalibrationDataMap {
	struct CborAdcCalibrationDataMap_float32float float32float[50];
	size_t float32float_count;
};

struct CborAdcChannelConfig {
	uint32_t interpolation_method;
	struct CborAdcCalibrationDataMap calibration_table;
	bool calibration_table_present;
};

struct CborAdcConfig {
	uint32_t samples;
	struct CborAdcChannelConfig CborAdcChannelConfig_m[24];
	size_t CborAdcChannelConfig_m_count;
};

#ifdef __cplusplus
}
#endif

#endif /* CBOR_ADC_CONFIG_H__ */
