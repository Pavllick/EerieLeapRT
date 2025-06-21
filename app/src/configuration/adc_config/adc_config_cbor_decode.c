/*
 * Generated using zcbor version 0.9.1
 * https://github.com/NordicSemiconductor/zcbor
 * Generated with a --default-max-qty of 24
 */

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include "zcbor_decode.h"
#include "adc_config_cbor_decode.h"
#include "zcbor_print.h"

#if DEFAULT_MAX_QTY != 24
#error "The type file was generated with a different default_max_qty than this file"
#endif

#define log_result(state, result, func) do { \
	if (!result) { \
		zcbor_trace_file(state); \
		zcbor_log("%s error: %s\r\n", func, zcbor_error_str(zcbor_peek_error(state))); \
	} else { \
		zcbor_log("%s success\r\n", func); \
	} \
} while(0)

static bool decode_repeated_AdcCalibrationDataMap_float32float(zcbor_state_t *state, struct AdcCalibrationDataMap_float32float *result);
static bool decode_AdcCalibrationDataMap(zcbor_state_t *state, struct AdcCalibrationDataMap *result);
static bool decode_AdcChannelConfig(zcbor_state_t *state, struct AdcChannelConfig *result);
static bool decode_AdcConfig(zcbor_state_t *state, struct AdcConfig *result);


static bool decode_repeated_AdcCalibrationDataMap_float32float(
		zcbor_state_t *state, struct AdcCalibrationDataMap_float32float *result)
{
	zcbor_log("%s\r\n", __func__);

	bool res = ((((zcbor_float32_decode(state, (&(*result).float32float_key))))
	&& (zcbor_float32_decode(state, (&(*result).float32float)))));

	log_result(state, res, __func__);
	return res;
}

static bool decode_AdcCalibrationDataMap(
		zcbor_state_t *state, struct AdcCalibrationDataMap *result)
{
	zcbor_log("%s\r\n", __func__);

	bool res = (((zcbor_map_start_decode(state) && ((zcbor_multi_decode(2, 50, &(*result).float32float_count, (zcbor_decoder_t *)decode_repeated_AdcCalibrationDataMap_float32float, state, (*&(*result).float32float), sizeof(struct AdcCalibrationDataMap_float32float))) || (zcbor_list_map_end_force_decode(state), false)) && zcbor_map_end_decode(state))));

	if (false) {
		/* For testing that the types of the arguments are correct.
		 * A compiler error here means a bug in zcbor.
		 */
		decode_repeated_AdcCalibrationDataMap_float32float(state, (*&(*result).float32float));
	}

	log_result(state, res, __func__);
	return res;
}

static bool decode_AdcChannelConfig(
		zcbor_state_t *state, struct AdcChannelConfig *result)
{
	zcbor_log("%s\r\n", __func__);

	bool res = (((zcbor_list_start_decode(state) && ((((zcbor_uint32_decode(state, (&(*result).interpolation_method))))
	&& ((*result).calibration_table_present = ((decode_AdcCalibrationDataMap(state, (&(*result).calibration_table)))), 1)) || (zcbor_list_map_end_force_decode(state), false)) && zcbor_list_end_decode(state))));

	log_result(state, res, __func__);
	return res;
}

static bool decode_AdcConfig(
		zcbor_state_t *state, struct AdcConfig *result)
{
	zcbor_log("%s\r\n", __func__);

	bool res = (((zcbor_list_start_decode(state) && ((((zcbor_uint32_decode(state, (&(*result).samples))))
	&& ((zcbor_list_start_decode(state) && ((zcbor_multi_decode(0, 24, &(*result).AdcChannelConfig_m_count, (zcbor_decoder_t *)decode_AdcChannelConfig, state, (*&(*result).AdcChannelConfig_m), sizeof(struct AdcChannelConfig))) || (zcbor_list_map_end_force_decode(state), false)) && zcbor_list_end_decode(state)))) || (zcbor_list_map_end_force_decode(state), false)) && zcbor_list_end_decode(state))));

	if (false) {
		/* For testing that the types of the arguments are correct.
		 * A compiler error here means a bug in zcbor.
		 */
		decode_AdcChannelConfig(state, (*&(*result).AdcChannelConfig_m));
	}

	log_result(state, res, __func__);
	return res;
}



int cbor_decode_AdcConfig(
		const uint8_t *payload, size_t payload_len,
		struct AdcConfig *result,
		size_t *payload_len_out)
{
	zcbor_state_t states[6];

	return zcbor_entry_function(payload, payload_len, (void *)result, payload_len_out, states,
		(zcbor_decoder_t *)decode_AdcConfig, sizeof(states) / sizeof(zcbor_state_t), 1);
}
