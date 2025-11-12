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

static bool encode_repeated_CborAdcCalibrationDataMap_float32float(zcbor_state_t *state, const struct CborAdcCalibrationDataMap_float32float *input);
static bool encode_CborAdcCalibrationDataMap(zcbor_state_t *state, const struct CborAdcCalibrationDataMap *input);
static bool encode_CborAdcChannelConfig(zcbor_state_t *state, const struct CborAdcChannelConfig *input);
static bool encode_CborAdcConfig(zcbor_state_t *state, const struct CborAdcConfig *input);


static bool encode_repeated_CborAdcCalibrationDataMap_float32float(
		zcbor_state_t *state, const struct CborAdcCalibrationDataMap_float32float *input)
{
	zcbor_log("%s\r\n", __func__);

	bool res = ((((zcbor_float32_encode(state, (&(*input).float32float_key))))
	&& (zcbor_float32_encode(state, (&(*input).float32float)))));

	log_result(state, res, __func__);
	return res;
}

static bool encode_CborAdcCalibrationDataMap(
		zcbor_state_t *state, const struct CborAdcCalibrationDataMap *input)
{
	zcbor_log("%s\r\n", __func__);

	bool res = (((zcbor_map_start_encode(state, 50) && ((zcbor_multi_encode_minmax(2, 50, &(*input).float32float_count, (zcbor_encoder_t *)encode_repeated_CborAdcCalibrationDataMap_float32float, state, (*&(*input).float32float), sizeof(struct CborAdcCalibrationDataMap_float32float))) || (zcbor_list_map_end_force_encode(state), false)) && zcbor_map_end_encode(state, 50))));

	log_result(state, res, __func__);
	return res;
}

static bool encode_CborAdcChannelConfig(
		zcbor_state_t *state, const struct CborAdcChannelConfig *input)
{
	zcbor_log("%s\r\n", __func__);

	bool res = (((zcbor_list_start_encode(state, 2) && ((((zcbor_uint32_encode(state, (&(*input).interpolation_method))))
	&& (!(*input).calibration_table_present || encode_CborAdcCalibrationDataMap(state, (&(*input).calibration_table)))) || (zcbor_list_map_end_force_encode(state), false)) && zcbor_list_end_encode(state, 2))));

	log_result(state, res, __func__);
	return res;
}

static bool encode_CborAdcConfig(
		zcbor_state_t *state, const struct CborAdcConfig *input)
{
	zcbor_log("%s\r\n", __func__);

	bool res = (((zcbor_list_start_encode(state, 3) && ((((zcbor_uint32_encode(state, (&(*input).samples))))
	&& ((zcbor_list_start_encode(state, 24) && ((zcbor_multi_encode_minmax(0, 24, &(*input).CborAdcChannelConfig_m_count, (zcbor_encoder_t *)encode_CborAdcChannelConfig, state, (*&(*input).CborAdcChannelConfig_m), sizeof(struct CborAdcChannelConfig))) || (zcbor_list_map_end_force_encode(state), false)) && zcbor_list_end_encode(state, 24)))
	&& ((zcbor_uint32_encode(state, (&(*input).json_config_checksum))))) || (zcbor_list_map_end_force_encode(state), false)) && zcbor_list_end_encode(state, 3))));

	log_result(state, res, __func__);
	return res;
}



int cbor_encode_CborAdcConfig(
		uint8_t *payload, size_t payload_len,
		const struct CborAdcConfig *input,
		size_t *payload_len_out)
{
	zcbor_state_t states[6];

	return zcbor_entry_function(payload, payload_len, (void *)input, payload_len_out, states,
		(zcbor_decoder_t *)encode_CborAdcConfig, sizeof(states) / sizeof(zcbor_state_t), 1);
}
