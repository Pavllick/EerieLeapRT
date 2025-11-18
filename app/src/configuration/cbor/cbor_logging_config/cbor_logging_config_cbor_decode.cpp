#include <cstdint>
#include <cstdbool>
#include <cstddef>
#include <cstring>
#include "zcbor_decode.h"
#include "cbor_logging_config_cbor_decode.h"
#include "zcbor_print.h"

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

	if (!zcbor_list_start_decode(state)) {
		return false;
	}

	if (!zcbor_uint32_decode(state, &result->logging_interval_ms)) {
		zcbor_list_end_decode(state);
		return false;
	}

	if (!zcbor_uint32_decode(state, &result->max_log_size_mb)) {
		zcbor_list_end_decode(state);
		return false;
	}

	if (!zcbor_list_start_decode(state)) {
		zcbor_list_end_decode(state);
		return false;
	}

	while (!zcbor_array_at_end(state)) {
		result->CborSensorLoggingConfig_m.emplace_back();
		if (!decode_CborSensorLoggingConfig(state, &result->CborSensorLoggingConfig_m.back())) {
			result->CborSensorLoggingConfig_m.pop_back();
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



int cbor_decode_CborLoggingConfig(
		const uint8_t *payload, size_t payload_len,
		struct CborLoggingConfig *result,
		size_t *payload_len_out)
{
	zcbor_state_t states[5];

	return zcbor_entry_function(payload, payload_len, (void *)result, payload_len_out, states,
		(zcbor_decoder_t *)decode_CborLoggingConfig, sizeof(states) / sizeof(zcbor_state_t), 1);
}
