/*
 * Generated using zcbor version 0.9.1
 * https://github.com/NordicSemiconductor/zcbor
 * Generated with a --default-max-qty of 24
 */

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include "zcbor_encode.h"
#include "adc_config_cbor_encode.h"
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

static bool encode_repeated_AdcCalibrationDataMap_float32float(zcbor_state_t *state, const struct AdcCalibrationDataMap_float32float *input);
static bool encode_AdcCalibrationDataMap(zcbor_state_t *state, const struct AdcCalibrationDataMap *input);
static bool encode_AdcConfig(zcbor_state_t *state, const struct AdcConfig *input);
static bool encode_AdcsConfig(zcbor_state_t *state, const struct AdcsConfig *input);


static bool encode_repeated_AdcCalibrationDataMap_float32float(
		zcbor_state_t *state, const struct AdcCalibrationDataMap_float32float *input)
{
	zcbor_log("%s\r\n", __func__);

	bool res = ((((zcbor_float32_encode(state, (&(*input).float32float_key))))
	&& (zcbor_float32_encode(state, (&(*input).float32float)))));

	log_result(state, res, __func__);
	return res;
}

static bool encode_AdcCalibrationDataMap(
		zcbor_state_t *state, const struct AdcCalibrationDataMap *input)
{
	zcbor_log("%s\r\n", __func__);

	bool res = (((zcbor_map_start_encode(state, 50) && ((zcbor_multi_encode_minmax(2, 50, &(*input).float32float_count, (zcbor_encoder_t *)encode_repeated_AdcCalibrationDataMap_float32float, state, (*&(*input).float32float), sizeof(struct AdcCalibrationDataMap_float32float))) || (zcbor_list_map_end_force_encode(state), false)) && zcbor_map_end_encode(state, 50))));

	log_result(state, res, __func__);
	return res;
}

static bool encode_AdcConfig(
		zcbor_state_t *state, const struct AdcConfig *input)
{
	zcbor_log("%s\r\n", __func__);

	bool res = (((zcbor_list_start_encode(state, 4) && ((((zcbor_tstr_encode(state, (&(*input).name))))
	&& ((zcbor_uint32_encode(state, (&(*input).samples))))
	&& ((zcbor_uint32_encode(state, (&(*input).interpolation_method))))
	&& (!(*input).calibration_table_present || encode_AdcCalibrationDataMap(state, (&(*input).calibration_table)))) || (zcbor_list_map_end_force_encode(state), false)) && zcbor_list_end_encode(state, 4))));

	log_result(state, res, __func__);
	return res;
}

static bool encode_AdcsConfig(
		zcbor_state_t *state, const struct AdcsConfig *input)
{
	zcbor_log("%s\r\n", __func__);

	bool res = (((zcbor_list_start_encode(state, 1) && ((((zcbor_list_start_encode(state, 24) && ((zcbor_multi_encode_minmax(0, 24, &(*input).AdcConfig_m_count, (zcbor_encoder_t *)encode_AdcConfig, state, (*&(*input).AdcConfig_m), sizeof(struct AdcConfig))) || (zcbor_list_map_end_force_encode(state), false)) && zcbor_list_end_encode(state, 24)))) || (zcbor_list_map_end_force_encode(state), false)) && zcbor_list_end_encode(state, 1))));

	log_result(state, res, __func__);
	return res;
}



int cbor_encode_AdcsConfig(
		uint8_t *payload, size_t payload_len,
		const struct AdcsConfig *input,
		size_t *payload_len_out)
{
	zcbor_state_t states[6];

	return zcbor_entry_function(payload, payload_len, (void *)input, payload_len_out, states,
		(zcbor_decoder_t *)encode_AdcsConfig, sizeof(states) / sizeof(zcbor_state_t), 1);
}
