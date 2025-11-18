#include <cstdint>
#include <cstdbool>
#include <cstddef>
#include <cstring>
#include "zcbor_decode.h"
#include "cbor_adc_config_cbor_decode.h"
#include "zcbor_print.h"

#define log_result(state, result, func) do { \
	if (!result) { \
		zcbor_trace_file(state); \
		zcbor_log("%s error: %s\r\n", func, zcbor_error_str(zcbor_peek_error(state))); \
	} else { \
		zcbor_log("%s success\r\n", func); \
	} \
} while(0)

static bool decode_repeated_CborAdcCalibrationDataMap_float32float(zcbor_state_t *state, struct CborAdcCalibrationDataMap_float32float *result);
static bool decode_CborAdcCalibrationDataMap(zcbor_state_t *state, struct CborAdcCalibrationDataMap *result);
static bool decode_CborAdcChannelConfig(zcbor_state_t *state, struct CborAdcChannelConfig *result);
static bool decode_CborAdcConfig(zcbor_state_t *state, struct CborAdcConfig *result);


static bool decode_repeated_CborAdcCalibrationDataMap_float32float(
		zcbor_state_t *state, struct CborAdcCalibrationDataMap_float32float *result)
{
	zcbor_log("%s\r\n", __func__);

	bool res = ((((zcbor_float32_decode(state, (&(*result).float32float_key))))
	&& (zcbor_float32_decode(state, (&(*result).float32float)))));

	log_result(state, res, __func__);
	return res;
}

static bool decode_CborAdcCalibrationDataMap(
		zcbor_state_t *state, struct CborAdcCalibrationDataMap *result)
{
	zcbor_log("%s\r\n", __func__);

	if (!zcbor_map_start_decode(state)) {
		return false;
	}

	while (!zcbor_array_at_end(state)) {
		result->float32float.emplace_back();
		if (!decode_repeated_CborAdcCalibrationDataMap_float32float(state, &result->float32float.back())) {
			result->float32float.pop_back();
			zcbor_list_map_end_force_decode(state);
			zcbor_map_end_decode(state);
			return false;
		}
	}

	if (!zcbor_map_end_decode(state)) {
		return false;
	}

	log_result(state, true, __func__);
	return true;
}

static bool decode_CborAdcChannelConfig(
		zcbor_state_t *state, struct CborAdcChannelConfig *result)
{
	zcbor_log("%s\r\n", __func__);

	bool res = (((zcbor_list_start_decode(state) && ((((zcbor_uint32_decode(state, (&(*result).interpolation_method))))
	&& ((*result).calibration_table_present = ((decode_CborAdcCalibrationDataMap(state, (&(*result).calibration_table)))), 1)) || (zcbor_list_map_end_force_decode(state), false)) && zcbor_list_end_decode(state))));

	log_result(state, res, __func__);
	return res;
}

static bool decode_CborAdcConfig(
		zcbor_state_t *state, struct CborAdcConfig *result)
{
	zcbor_log("%s\r\n", __func__);

	if (!zcbor_list_start_decode(state)) {
		return false;
	}

	if (!zcbor_uint32_decode(state, &result->samples)) {
		zcbor_list_end_decode(state);
		return false;
	}

	if (!zcbor_list_start_decode(state)) {
		zcbor_list_end_decode(state);
		return false;
	}

	while (!zcbor_array_at_end(state)) {
		result->CborAdcChannelConfig_m.emplace_back();
		if (!decode_CborAdcChannelConfig(state, &result->CborAdcChannelConfig_m.back())) {
			result->CborAdcChannelConfig_m.pop_back();
			zcbor_list_map_end_force_decode(state);
			zcbor_list_end_decode(state);
			zcbor_list_end_decode(state);
			return false;
		}
	}

	if (!zcbor_list_end_decode(state)) {
		zcbor_list_end_decode(state);
		return false;
	}

	if (!zcbor_uint32_decode(state, &result->json_config_checksum)) {
		zcbor_list_end_decode(state);
		return false;
	}

	if (!zcbor_list_end_decode(state)) {
		return false;
	}

	log_result(state, true, __func__);
	return true;
}



int cbor_decode_CborAdcConfig(
		const uint8_t *payload, size_t payload_len,
		struct CborAdcConfig *result,
		size_t *payload_len_out)
{
	zcbor_state_t states[6];

	return zcbor_entry_function(payload, payload_len, (void *)result, payload_len_out, states,
		(zcbor_decoder_t *)decode_CborAdcConfig, sizeof(states) / sizeof(zcbor_state_t), 1);
}
