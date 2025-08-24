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
#include "system_config_cbor_decode.h"
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

static bool decode_ComUserConfig(zcbor_state_t *state, struct ComUserConfig *result);
static bool decode_SystemConfig(zcbor_state_t *state, struct SystemConfig *result);


static bool decode_ComUserConfig(
		zcbor_state_t *state, struct ComUserConfig *result)
{
	zcbor_log("%s\r\n", __func__);

	bool res = (((zcbor_list_start_decode(state) && ((((zcbor_uint64_decode(state, (&(*result).device_id))))
	&& ((zcbor_uint32_decode(state, (&(*result).server_id))))) || (zcbor_list_map_end_force_decode(state), false)) && zcbor_list_end_decode(state))));

	log_result(state, res, __func__);
	return res;
}

static bool decode_SystemConfig(
		zcbor_state_t *state, struct SystemConfig *result)
{
	zcbor_log("%s\r\n", __func__);

	bool res = (((zcbor_list_start_decode(state) && ((((zcbor_uint64_decode(state, (&(*result).device_id))))
	&& ((zcbor_uint32_decode(state, (&(*result).hw_version))))
	&& ((zcbor_uint32_decode(state, (&(*result).sw_version))))
	&& ((zcbor_uint32_decode(state, (&(*result).build_number))))
	&& ((zcbor_list_start_decode(state) && ((zcbor_multi_decode(0, 24, &(*result).ComUserConfig_m_count, (zcbor_decoder_t *)decode_ComUserConfig, state, (*&(*result).ComUserConfig_m), sizeof(struct ComUserConfig))) || (zcbor_list_map_end_force_decode(state), false)) && zcbor_list_end_decode(state)))) || (zcbor_list_map_end_force_decode(state), false)) && zcbor_list_end_decode(state))));

	if (false) {
		/* For testing that the types of the arguments are correct.
		 * A compiler error here means a bug in zcbor.
		 */
		decode_ComUserConfig(state, (*&(*result).ComUserConfig_m));
	}

	log_result(state, res, __func__);
	return res;
}



int cbor_decode_SystemConfig(
		const uint8_t *payload, size_t payload_len,
		struct SystemConfig *result,
		size_t *payload_len_out)
{
	zcbor_state_t states[5];

	return zcbor_entry_function(payload, payload_len, (void *)result, payload_len_out, states,
		(zcbor_decoder_t *)decode_SystemConfig, sizeof(states) / sizeof(zcbor_state_t), 1);
}
