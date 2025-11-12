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

static bool encode_CborSensorLoggingConfig(zcbor_state_t *state, const struct CborSensorLoggingConfig *input);
static bool encode_CborLoggingConfig(zcbor_state_t *state, const struct CborLoggingConfig *input);


static bool encode_CborSensorLoggingConfig(
		zcbor_state_t *state, const struct CborSensorLoggingConfig *input)
{
	zcbor_log("%s\r\n", __func__);

	bool res = (((zcbor_list_start_encode(state, 4) && ((((zcbor_uint32_encode(state, (&(*input).sensor_id_hash))))
	&& ((zcbor_bool_encode(state, (&(*input).is_enabled))))
	&& ((zcbor_bool_encode(state, (&(*input).log_raw_value))))
	&& ((zcbor_bool_encode(state, (&(*input).log_only_new_data))))) || (zcbor_list_map_end_force_encode(state), false)) && zcbor_list_end_encode(state, 4))));

	log_result(state, res, __func__);
	return res;
}

static bool encode_CborLoggingConfig(
		zcbor_state_t *state, const struct CborLoggingConfig *input)
{
	zcbor_log("%s\r\n", __func__);

	bool res = (((zcbor_list_start_encode(state, 4) && ((((zcbor_uint32_encode(state, (&(*input).logging_interval_ms))))
	&& ((zcbor_uint32_encode(state, (&(*input).max_log_size_mb))))
	&& ((zcbor_list_start_encode(state, 24) && ((zcbor_multi_encode_minmax(0, 24, &(*input).CborSensorLoggingConfig_m_count, (zcbor_encoder_t *)encode_CborSensorLoggingConfig, state, (*&(*input).CborSensorLoggingConfig_m), sizeof(struct CborSensorLoggingConfig))) || (zcbor_list_map_end_force_encode(state), false)) && zcbor_list_end_encode(state, 24)))
	&& ((zcbor_uint32_encode(state, (&(*input).json_config_checksum))))) || (zcbor_list_map_end_force_encode(state), false)) && zcbor_list_end_encode(state, 4))));

	log_result(state, res, __func__);
	return res;
}



int cbor_encode_CborLoggingConfig(
		uint8_t *payload, size_t payload_len,
		const struct CborLoggingConfig *input,
		size_t *payload_len_out)
{
	zcbor_state_t states[5];

	return zcbor_entry_function(payload, payload_len, (void *)input, payload_len_out, states,
		(zcbor_decoder_t *)encode_CborLoggingConfig, sizeof(states) / sizeof(zcbor_state_t), 1);
}
