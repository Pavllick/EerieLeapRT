#include <cstdint>
#include <cstdbool>
#include <cstddef>
#include <cstring>
#include "zcbor_encode.h"
#include "cbor_sensors_config_cbor_encode.h"
#include "zcbor_print.h"

#define log_result(state, result, func) do { \
	if (!result) { \
		zcbor_trace_file(state); \
		zcbor_log("%s error: %s\r\n", func, zcbor_error_str(zcbor_peek_error(state))); \
	} else { \
		zcbor_log("%s success\r\n", func); \
	} \
} while(0)

static bool encode_CborSensorMetadataConfig(zcbor_state_t *state, const struct CborSensorMetadataConfig *input);
static bool encode_repeated_CborSensorCalibrationDataMap_float32float(zcbor_state_t *state, const struct CborSensorCalibrationDataMap_float32float *input);
static bool encode_CborSensorCalibrationDataMap(zcbor_state_t *state, const struct CborSensorCalibrationDataMap *input);
static bool encode_CborSensorConfigurationConfig(zcbor_state_t *state, const struct CborSensorConfigurationConfig *input);
static bool encode_CborSensorConfig(zcbor_state_t *state, const struct CborSensorConfig *input);
static bool encode_CborSensorsConfig(zcbor_state_t *state, const struct CborSensorsConfig *input);


static bool encode_CborSensorMetadataConfig(
		zcbor_state_t *state, const struct CborSensorMetadataConfig *input)
{
	zcbor_log("%s\r\n", __func__);

	bool res = (((zcbor_list_start_encode(state, 3) && ((((zcbor_tstr_encode(state, (&(*input).name))))
	&& ((zcbor_tstr_encode(state, (&(*input).unit))))
	&& ((zcbor_tstr_encode(state, (&(*input).description))))) || (zcbor_list_map_end_force_encode(state), false)) && zcbor_list_end_encode(state, 3))));

	log_result(state, res, __func__);
	return res;
}

static bool encode_repeated_CborSensorCalibrationDataMap_float32float(
		zcbor_state_t *state, const struct CborSensorCalibrationDataMap_float32float *input)
{
	zcbor_log("%s\r\n", __func__);

	bool res = ((((zcbor_float32_encode(state, (&(*input).float32float_key))))
	&& (zcbor_float32_encode(state, (&(*input).float32float)))));

	log_result(state, res, __func__);
	return res;
}

static bool encode_CborSensorCalibrationDataMap(
		zcbor_state_t *state, const struct CborSensorCalibrationDataMap *input)
{
	zcbor_log("%s\r\n", __func__);

	size_t float32float_count = input->float32float.size();

	bool res = (((zcbor_map_start_encode(state, float32float_count) && ((zcbor_multi_encode_minmax(2, float32float_count, &float32float_count, (zcbor_encoder_t *)encode_repeated_CborSensorCalibrationDataMap_float32float, state, input->float32float.data(), sizeof(struct CborSensorCalibrationDataMap_float32float))) || (zcbor_list_map_end_force_encode(state), false)) && zcbor_map_end_encode(state, float32float_count))));

	log_result(state, res, __func__);
	return res;
}

static bool encode_CborSensorConfigurationConfig(
		zcbor_state_t *state, const struct CborSensorConfigurationConfig *input)
{
	zcbor_log("%s\r\n", __func__);

	bool res = (((zcbor_list_start_encode(state, 7) && ((((zcbor_uint32_encode(state, (&(*input).type))))
	&& ((zcbor_uint32_encode(state, (&(*input).sampling_rate_ms))))
	&& ((zcbor_uint32_encode(state, (&(*input).interpolation_method))))
	&& (!(*input).channel_present || zcbor_uint32_encode(state, (&(*input).channel)))
	&& ((zcbor_tstr_encode(state, (&(*input).connection_string))))
	&& ((zcbor_tstr_encode(state, (&(*input).script_path))))
	&& (!(*input).calibration_table_present || encode_CborSensorCalibrationDataMap(state, (&(*input).calibration_table)))
	&& (!(*input).expression_present || zcbor_tstr_encode(state, (&(*input).expression)))) || (zcbor_list_map_end_force_encode(state), false)) && zcbor_list_end_encode(state, 7))));

	log_result(state, res, __func__);
	return res;
}

static bool encode_CborSensorConfig(
		zcbor_state_t *state, const struct CborSensorConfig *input)
{
	zcbor_log("%s\r\n", __func__);

	bool res = (((zcbor_list_start_encode(state, 3) && ((((zcbor_tstr_encode(state, (&(*input).id))))
	&& ((encode_CborSensorMetadataConfig(state, (&(*input).metadata))))
	&& ((encode_CborSensorConfigurationConfig(state, (&(*input).configuration))))) || (zcbor_list_map_end_force_encode(state), false)) && zcbor_list_end_encode(state, 3))));

	log_result(state, res, __func__);
	return res;
}

static bool encode_CborSensorsConfig(
		zcbor_state_t *state, const struct CborSensorsConfig *input)
{
	zcbor_log("%s\r\n", __func__);

	size_t CborSensorConfig_m_count = input->CborSensorConfig_m.size();

	bool res = (((zcbor_list_start_encode(state, 2) && ((((zcbor_list_start_encode(state, CborSensorConfig_m_count) && ((zcbor_multi_encode_minmax(0, CborSensorConfig_m_count, &CborSensorConfig_m_count, (zcbor_encoder_t *)encode_CborSensorConfig, state, input->CborSensorConfig_m.data(), sizeof(struct CborSensorConfig))) || (zcbor_list_map_end_force_encode(state), false)) && zcbor_list_end_encode(state, CborSensorConfig_m_count)))
	&& ((zcbor_uint32_encode(state, (&(*input).json_config_checksum))))) || (zcbor_list_map_end_force_encode(state), false)) && zcbor_list_end_encode(state, 2))));

	log_result(state, res, __func__);
	return res;
}



int cbor_encode_CborSensorsConfig(
		uint8_t *payload, size_t payload_len,
		const struct CborSensorsConfig *input,
		size_t *payload_len_out)
{
	zcbor_state_t states[7];

	return zcbor_entry_function(payload, payload_len, (void *)input, payload_len_out, states,
		(zcbor_decoder_t *)encode_CborSensorsConfig, sizeof(states) / sizeof(zcbor_state_t), 1);
}
