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
#include "logging_config_cbor_decode.h"
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

static bool decode_CborSensorLoggingConfig(zcbor_state_t *state, struct CborSensorLoggingConfig *result);
static bool decode_CborLoggingConfig(zcbor_state_t *state, struct CborLoggingConfig *result);


static bool decode_CborSensorLoggingConfig(
		zcbor_state_t *state, struct CborSensorLoggingConfig *result)
{
	zcbor_log("%s\r\n", __func__);

	bool res = (((zcbor_list_start_decode(state) && ((((zcbor_uint32_decode(state, (&(*result).sensor_id_hash))))
	&& ((zcbor_bool_decode(state, (&(*result).is_enabled))))
	&& ((zcbor_bool_decode(state, (&(*result).log_raw_value))))
	&& ((zcbor_bool_decode(state, (&(*result).log_only_new_data))))) || (zcbor_list_map_end_force_decode(state), false)) && zcbor_list_end_decode(state))));

	log_result(state, res, __func__);
	return res;
}

static bool decode_CborLoggingConfig(
		zcbor_state_t *state, struct CborLoggingConfig *result)
{
	zcbor_log("%s\r\n", __func__);

	bool res = (((zcbor_list_start_decode(state) && ((((zcbor_uint32_decode(state, (&(*result).logging_interval_ms))))
	&& ((zcbor_uint32_decode(state, (&(*result).max_log_size_mb))))
	&& ((zcbor_list_start_decode(state) && ((zcbor_multi_decode(0, 24, &(*result).CborSensorLoggingConfig_m_count, (zcbor_decoder_t *)decode_CborSensorLoggingConfig, state, (*&(*result).CborSensorLoggingConfig_m), sizeof(struct CborSensorLoggingConfig))) || (zcbor_list_map_end_force_decode(state), false)) && zcbor_list_end_decode(state)))) || (zcbor_list_map_end_force_decode(state), false)) && zcbor_list_end_decode(state))));

	if (false) {
		/* For testing that the types of the arguments are correct.
		 * A compiler error here means a bug in zcbor.
		 */
		decode_CborSensorLoggingConfig(state, (*&(*result).CborSensorLoggingConfig_m));
	}

	log_result(state, res, __func__);
	return res;
}



int cbor_decode_CborLoggingConfig(
		const uint8_t *payload, size_t payload_len,
		struct CborLoggingConfig *result,
		size_t *payload_len_out)
{
	zcbor_state_t states[5];

	return zcbor_entry_function(payload, payload_len, (void *)result, payload_len_out, states,
		(zcbor_decoder_t *)decode_CborLoggingConfig, sizeof(states) / sizeof(zcbor_state_t), 1);
}
