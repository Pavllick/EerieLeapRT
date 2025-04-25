/*
 * Generated using zcbor version 0.9.1
 * https://github.com/NordicSemiconductor/zcbor
 * Generated with a --default-max-qty of 40
 */

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include "zcbor_encode.h"
#include "sensor_config_cbor_encode.h"
#include "zcbor_print.h"

#if DEFAULT_MAX_QTY != 40
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

static bool encode_SensorMetadataConfig(zcbor_state_t *state, const struct SensorMetadataConfig *input);
static bool encode_repeated_SensorCalibrationDataMap_floatfloat(zcbor_state_t *state, const struct SensorCalibrationDataMap_floatfloat *input);
static bool encode_SensorCalibrationDataMap(zcbor_state_t *state, const struct SensorCalibrationDataMap *input);
static bool encode_SensorConfigurationConfig(zcbor_state_t *state, const struct SensorConfigurationConfig *input);
static bool encode_SensorConfig(zcbor_state_t *state, const struct SensorConfig *input);
static bool encode_SensorsConfig(zcbor_state_t *state, const struct SensorsConfig *input);


static bool encode_SensorMetadataConfig(
		zcbor_state_t *state, const struct SensorMetadataConfig *input)
{
	zcbor_log("%s\r\n", __func__);

	bool res = (((zcbor_list_start_encode(state, 3) && ((((zcbor_tstr_encode(state, (&(*input).name))))
	&& ((zcbor_tstr_encode(state, (&(*input).unit))))
	&& (!(*input).description_present || zcbor_tstr_encode(state, (&(*input).description)))) || (zcbor_list_map_end_force_encode(state), false)) && zcbor_list_end_encode(state, 3))));

	log_result(state, res, __func__);
	return res;
}

static bool encode_repeated_SensorCalibrationDataMap_floatfloat(
		zcbor_state_t *state, const struct SensorCalibrationDataMap_floatfloat *input)
{
	zcbor_log("%s\r\n", __func__);

	bool res = ((((zcbor_float64_encode(state, (&(*input).floatfloat_key))))
	&& (zcbor_float64_encode(state, (&(*input).floatfloat)))));

	log_result(state, res, __func__);
	return res;
}

static bool encode_SensorCalibrationDataMap(
		zcbor_state_t *state, const struct SensorCalibrationDataMap *input)
{
	zcbor_log("%s\r\n", __func__);

	bool res = (((zcbor_map_start_encode(state, 100) && ((zcbor_multi_encode_minmax(2, 100, &(*input).floatfloat_count, (zcbor_encoder_t *)encode_repeated_SensorCalibrationDataMap_floatfloat, state, (*&(*input).floatfloat), sizeof(struct SensorCalibrationDataMap_floatfloat))) || (zcbor_list_map_end_force_encode(state), false)) && zcbor_map_end_encode(state, 100))));

	log_result(state, res, __func__);
	return res;
}

static bool encode_SensorConfigurationConfig(
		zcbor_state_t *state, const struct SensorConfigurationConfig *input)
{
	zcbor_log("%s\r\n", __func__);

	bool res = (((zcbor_list_start_encode(state, 5) && ((((zcbor_uint32_encode(state, (&(*input).type))))
	&& (!(*input).channel_present || zcbor_uint32_encode(state, (&(*input).channel)))
	&& (!(*input).sampling_rate_ms_present || zcbor_uint32_encode(state, (&(*input).sampling_rate_ms)))
	&& (!(*input).calibration_table_present || encode_SensorCalibrationDataMap(state, (&(*input).calibration_table)))
	&& (!(*input).expression_present || zcbor_tstr_encode(state, (&(*input).expression)))) || (zcbor_list_map_end_force_encode(state), false)) && zcbor_list_end_encode(state, 5))));

	log_result(state, res, __func__);
	return res;
}

static bool encode_SensorConfig(
		zcbor_state_t *state, const struct SensorConfig *input)
{
	zcbor_log("%s\r\n", __func__);

	bool res = (((zcbor_list_start_encode(state, 3) && ((((zcbor_tstr_encode(state, (&(*input).id))))
	&& ((encode_SensorMetadataConfig(state, (&(*input).metadata))))
	&& ((encode_SensorConfigurationConfig(state, (&(*input).configuration))))) || (zcbor_list_map_end_force_encode(state), false)) && zcbor_list_end_encode(state, 3))));

	log_result(state, res, __func__);
	return res;
}

static bool encode_SensorsConfig(
		zcbor_state_t *state, const struct SensorsConfig *input)
{
	zcbor_log("%s\r\n", __func__);

	bool res = (((zcbor_list_start_encode(state, 1) && ((((zcbor_list_start_encode(state, 40) && ((zcbor_multi_encode_minmax(0, 40, &(*input).SensorConfig_m_count, (zcbor_encoder_t *)encode_SensorConfig, state, (*&(*input).SensorConfig_m), sizeof(struct SensorConfig))) || (zcbor_list_map_end_force_encode(state), false)) && zcbor_list_end_encode(state, 40)))) || (zcbor_list_map_end_force_encode(state), false)) && zcbor_list_end_encode(state, 1))));

	log_result(state, res, __func__);
	return res;
}



int cbor_encode_SensorsConfig(
		uint8_t *payload, size_t payload_len,
		const struct SensorsConfig *input,
		size_t *payload_len_out)
{
	zcbor_state_t states[7];

	return zcbor_entry_function(payload, payload_len, (void *)input, payload_len_out, states,
		(zcbor_decoder_t *)encode_SensorsConfig, sizeof(states) / sizeof(zcbor_state_t), 1);
}
