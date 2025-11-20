#include <cstdint>
#include <cstdbool>
#include <cstddef>
#include <cstring>
#include "zcbor_decode.h"
#include "cbor_sensors_config_cbor_decode.h"
#include "zcbor_print.h"

#define log_result(state, result, func) do { \
	if (!result) { \
		zcbor_trace_file(state); \
		zcbor_log("%s error: %s\r\n", func, zcbor_error_str(zcbor_peek_error(state))); \
	} else { \
		zcbor_log("%s success\r\n", func); \
	} \
} while(0)

static bool decode_CborSensorMetadataConfig(zcbor_state_t *state, struct CborSensorMetadataConfig *result);
static bool decode_repeated_CborSensorCalibrationDataMap_float32float(zcbor_state_t *state, struct CborSensorCalibrationDataMap_float32float *result);
static bool decode_CborSensorCalibrationDataMap(zcbor_state_t *state, struct CborSensorCalibrationDataMap *result);
static bool decode_CborSensorConfigurationConfig(zcbor_state_t *state, struct CborSensorConfigurationConfig *result);
static bool decode_CborSensorConfig(zcbor_state_t *state, struct CborSensorConfig *result);
static bool decode_CborSensorsConfig(zcbor_state_t *state, struct CborSensorsConfig *result);


static bool decode_CborSensorMetadataConfig(
		zcbor_state_t *state, struct CborSensorMetadataConfig *result)
{
	zcbor_log("%s\r\n", __func__);

	bool res = (((zcbor_list_start_decode(state) && ((((zcbor_tstr_decode(state, (&(*result).name))))
	&& ((zcbor_tstr_decode(state, (&(*result).unit))))
	&& ((zcbor_tstr_decode(state, (&(*result).description))))) || (zcbor_list_map_end_force_decode(state), false)) && zcbor_list_end_decode(state))));

	log_result(state, res, __func__);
	return res;
}

static bool decode_repeated_CborSensorCalibrationDataMap_float32float(
		zcbor_state_t *state, struct CborSensorCalibrationDataMap_float32float *result)
{
	zcbor_log("%s\r\n", __func__);

	bool res = ((((zcbor_float32_decode(state, (&(*result).float32float_key))))
	&& (zcbor_float32_decode(state, (&(*result).float32float)))));

	log_result(state, res, __func__);
	return res;
}

static bool decode_CborSensorCalibrationDataMap(
		zcbor_state_t *state, struct CborSensorCalibrationDataMap *result)
{
	zcbor_log("%s\r\n", __func__);

	if (!zcbor_map_start_decode(state)) {
		return false;
	}

	while (!zcbor_array_at_end(state)) {
		result->float32float.emplace_back();
		if (!decode_repeated_CborSensorCalibrationDataMap_float32float(state, &result->float32float.back())) {
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

static bool decode_CborSensorConfigurationConfig(
		zcbor_state_t *state, struct CborSensorConfigurationConfig *result)
{
	zcbor_log("%s\r\n", __func__);

	bool res = (((zcbor_list_start_decode(state) && ((((zcbor_uint32_decode(state, (&(*result).type))))
	&& ((zcbor_uint32_decode(state, (&(*result).sampling_rate_ms))))
	&& ((zcbor_uint32_decode(state, (&(*result).interpolation_method))))
	&& ((*result).channel_present = ((zcbor_uint32_decode(state, (&(*result).channel)))), 1)
	&& ((zcbor_tstr_decode(state, (&(*result).connection_string))))
	&& ((zcbor_tstr_decode(state, (&(*result).script_path))))
	&& ((*result).calibration_table_present = ((decode_CborSensorCalibrationDataMap(state, (&(*result).calibration_table)))), 1)
	&& ((*result).expression_present = ((zcbor_tstr_decode(state, (&(*result).expression)))), 1)) || (zcbor_list_map_end_force_decode(state), false)) && zcbor_list_end_decode(state))));

	log_result(state, res, __func__);
	return res;
}

static bool decode_CborSensorConfig(
		zcbor_state_t *state, struct CborSensorConfig *result)
{
	zcbor_log("%s\r\n", __func__);

	bool res = (((zcbor_list_start_decode(state) && ((((zcbor_tstr_decode(state, (&(*result).id))))
	&& ((decode_CborSensorMetadataConfig(state, (&(*result).metadata))))
	&& ((decode_CborSensorConfigurationConfig(state, (&(*result).configuration))))) || (zcbor_list_map_end_force_decode(state), false)) && zcbor_list_end_decode(state))));

	log_result(state, res, __func__);
	return res;
}

static bool decode_CborSensorsConfig(
		zcbor_state_t *state, struct CborSensorsConfig *result)
{
	zcbor_log("%s\r\n", __func__);

	if (!zcbor_list_start_decode(state)) {
		return false;
	}

	if (!zcbor_list_start_decode(state)) {
		zcbor_list_end_decode(state);
		return false;
	}

	while (!zcbor_array_at_end(state)) {
		result->CborSensorConfig_m.emplace_back();
		if (!decode_CborSensorConfig(state, &result->CborSensorConfig_m.back())) {
			result->CborSensorConfig_m.pop_back();
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



int cbor_decode_CborSensorsConfig(
		const uint8_t *payload, size_t payload_len,
		struct CborSensorsConfig *result,
		size_t *payload_len_out)
{
	zcbor_state_t states[7];

	return zcbor_entry_function(payload, payload_len, (void *)result, payload_len_out, states,
		(zcbor_decoder_t *)decode_CborSensorsConfig, sizeof(states) / sizeof(zcbor_state_t), 1);
}
