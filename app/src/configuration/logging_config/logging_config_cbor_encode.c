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
#include "logging_config_cbor_encode.h"
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

static bool encode_SensorLoggingConfig(zcbor_state_t *state, const struct SensorLoggingConfig *input);
static bool encode_repeated_SensorLoggingConfigMap_SensorLoggingConfig_m(zcbor_state_t *state, const struct SensorLoggingConfigMap_SensorLoggingConfig_m *input);
static bool encode_SensorLoggingConfigMap(zcbor_state_t *state, const struct SensorLoggingConfigMap *input);
static bool encode_LoggingConfig(zcbor_state_t *state, const struct LoggingConfig *input);


static bool encode_SensorLoggingConfig(
		zcbor_state_t *state, const struct SensorLoggingConfig *input)
{
	zcbor_log("%s\r\n", __func__);

	bool res = (((zcbor_list_start_encode(state, 3) && ((((zcbor_bool_encode(state, (&(*input).is_enabled))))
	&& ((zcbor_bool_encode(state, (&(*input).log_raw_value))))
	&& ((zcbor_bool_encode(state, (&(*input).log_only_new_data))))) || (zcbor_list_map_end_force_encode(state), false)) && zcbor_list_end_encode(state, 3))));

	log_result(state, res, __func__);
	return res;
}

static bool encode_repeated_SensorLoggingConfigMap_SensorLoggingConfig_m(
		zcbor_state_t *state, const struct SensorLoggingConfigMap_SensorLoggingConfig_m *input)
{
	zcbor_log("%s\r\n", __func__);

	bool res = ((((zcbor_uint32_encode(state, (&(*input).SensorLoggingConfig_m_key))))
	&& (encode_SensorLoggingConfig(state, (&(*input).SensorLoggingConfig_m)))));

	log_result(state, res, __func__);
	return res;
}

static bool encode_SensorLoggingConfigMap(
		zcbor_state_t *state, const struct SensorLoggingConfigMap *input)
{
	zcbor_log("%s\r\n", __func__);

	bool res = (((zcbor_map_start_encode(state, 24) && ((zcbor_multi_encode_minmax(0, 24, &(*input).SensorLoggingConfig_m_count, (zcbor_encoder_t *)encode_repeated_SensorLoggingConfigMap_SensorLoggingConfig_m, state, (*&(*input).SensorLoggingConfig_m), sizeof(struct SensorLoggingConfigMap_SensorLoggingConfig_m))) || (zcbor_list_map_end_force_encode(state), false)) && zcbor_map_end_encode(state, 24))));

	log_result(state, res, __func__);
	return res;
}

static bool encode_LoggingConfig(
		zcbor_state_t *state, const struct LoggingConfig *input)
{
	zcbor_log("%s\r\n", __func__);

	bool res = (((zcbor_list_start_encode(state, 3) && ((((zcbor_uint32_encode(state, (&(*input).logging_interval_ms))))
	&& ((zcbor_uint32_encode(state, (&(*input).max_log_size_mb))))
	&& ((encode_SensorLoggingConfigMap(state, (&(*input).sensor_configurations))))) || (zcbor_list_map_end_force_encode(state), false)) && zcbor_list_end_encode(state, 3))));

	log_result(state, res, __func__);
	return res;
}



int cbor_encode_LoggingConfig(
		uint8_t *payload, size_t payload_len,
		const struct LoggingConfig *input,
		size_t *payload_len_out)
{
	zcbor_state_t states[5];

	return zcbor_entry_function(payload, payload_len, (void *)input, payload_len_out, states,
		(zcbor_decoder_t *)encode_LoggingConfig, sizeof(states) / sizeof(zcbor_state_t), 1);
}
