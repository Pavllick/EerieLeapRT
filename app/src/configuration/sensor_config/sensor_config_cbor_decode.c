/*
 * Generated using zcbor version 0.9.1
 * https://github.com/NordicSemiconductor/zcbor
 * Generated with a --default-max-qty of 40
 */

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include "zcbor_decode.h"
#include "sensor_config_cbor_decode.h"
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

static bool decode_SensorMetadataConfig(zcbor_state_t *state, struct SensorMetadataConfig *result);
static bool decode_repeated_SensorCalibrationDataMap_floatfloat(zcbor_state_t *state, struct SensorCalibrationDataMap_floatfloat *result);
static bool decode_SensorCalibrationDataMap(zcbor_state_t *state, struct SensorCalibrationDataMap *result);
static bool decode_SensorConfigurationConfig(zcbor_state_t *state, struct SensorConfigurationConfig *result);
static bool decode_SensorConfig(zcbor_state_t *state, struct SensorConfig *result);
static bool decode_SensorsConfig(zcbor_state_t *state, struct SensorsConfig *result);


static bool decode_SensorMetadataConfig(
		zcbor_state_t *state, struct SensorMetadataConfig *result)
{
	zcbor_log("%s\r\n", __func__);

	bool res = (((zcbor_list_start_decode(state) && ((((zcbor_tstr_decode(state, (&(*result).name))))
	&& ((zcbor_tstr_decode(state, (&(*result).unit))))
	&& ((*result).description_present = ((zcbor_tstr_decode(state, (&(*result).description)))), 1)) || (zcbor_list_map_end_force_decode(state), false)) && zcbor_list_end_decode(state))));

	log_result(state, res, __func__);
	return res;
}

static bool decode_repeated_SensorCalibrationDataMap_floatfloat(
		zcbor_state_t *state, struct SensorCalibrationDataMap_floatfloat *result)
{
	zcbor_log("%s\r\n", __func__);

	bool res = ((((zcbor_float_decode(state, (&(*result).floatfloat_key))))
	&& (zcbor_float_decode(state, (&(*result).floatfloat)))));

	log_result(state, res, __func__);
	return res;
}

static bool decode_SensorCalibrationDataMap(
		zcbor_state_t *state, struct SensorCalibrationDataMap *result)
{
	zcbor_log("%s\r\n", __func__);

	bool res = (((zcbor_map_start_decode(state) && ((zcbor_multi_decode(2, 100, &(*result).floatfloat_count, (zcbor_decoder_t *)decode_repeated_SensorCalibrationDataMap_floatfloat, state, (*&(*result).floatfloat), sizeof(struct SensorCalibrationDataMap_floatfloat))) || (zcbor_list_map_end_force_decode(state), false)) && zcbor_map_end_decode(state))));

	if (false) {
		/* For testing that the types of the arguments are correct.
		 * A compiler error here means a bug in zcbor.
		 */
		decode_repeated_SensorCalibrationDataMap_floatfloat(state, (*&(*result).floatfloat));
	}

	log_result(state, res, __func__);
	return res;
}

static bool decode_SensorConfigurationConfig(
		zcbor_state_t *state, struct SensorConfigurationConfig *result)
{
	zcbor_log("%s\r\n", __func__);

	bool res = (((zcbor_list_start_decode(state) && ((((zcbor_uint32_decode(state, (&(*result).type))))
	&& ((*result).channel_present = ((zcbor_uint32_decode(state, (&(*result).channel)))), 1)
	&& ((*result).sampling_rate_ms_present = ((zcbor_uint32_decode(state, (&(*result).sampling_rate_ms)))), 1)
	&& ((*result).calibration_table_present = ((decode_SensorCalibrationDataMap(state, (&(*result).calibration_table)))), 1)
	&& ((zcbor_uint32_decode(state, (&(*result).interpolation_method))))
	&& ((*result).expression_present = ((zcbor_tstr_decode(state, (&(*result).expression)))), 1)) || (zcbor_list_map_end_force_decode(state), false)) && zcbor_list_end_decode(state))));

	log_result(state, res, __func__);
	return res;
}

static bool decode_SensorConfig(
		zcbor_state_t *state, struct SensorConfig *result)
{
	zcbor_log("%s\r\n", __func__);

	bool res = (((zcbor_list_start_decode(state) && ((((zcbor_tstr_decode(state, (&(*result).id))))
	&& ((decode_SensorMetadataConfig(state, (&(*result).metadata))))
	&& ((decode_SensorConfigurationConfig(state, (&(*result).configuration))))) || (zcbor_list_map_end_force_decode(state), false)) && zcbor_list_end_decode(state))));

	log_result(state, res, __func__);
	return res;
}

static bool decode_SensorsConfig(
		zcbor_state_t *state, struct SensorsConfig *result)
{
	zcbor_log("%s\r\n", __func__);

	bool res = (((zcbor_list_start_decode(state) && ((((zcbor_list_start_decode(state) && ((zcbor_multi_decode(0, 40, &(*result).SensorConfig_m_count, (zcbor_decoder_t *)decode_SensorConfig, state, (*&(*result).SensorConfig_m), sizeof(struct SensorConfig))) || (zcbor_list_map_end_force_decode(state), false)) && zcbor_list_end_decode(state)))) || (zcbor_list_map_end_force_decode(state), false)) && zcbor_list_end_decode(state))));

	if (false) {
		/* For testing that the types of the arguments are correct.
		 * A compiler error here means a bug in zcbor.
		 */
		decode_SensorConfig(state, (*&(*result).SensorConfig_m));
	}

	log_result(state, res, __func__);
	return res;
}



int cbor_decode_SensorsConfig(
		const uint8_t *payload, size_t payload_len,
		struct SensorsConfig *result,
		size_t *payload_len_out)
{
	zcbor_state_t states[7];

	return zcbor_entry_function(payload, payload_len, (void *)result, payload_len_out, states,
		(zcbor_decoder_t *)decode_SensorsConfig, sizeof(states) / sizeof(zcbor_state_t), 1);
}
