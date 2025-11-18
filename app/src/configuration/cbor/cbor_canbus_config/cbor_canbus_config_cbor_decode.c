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
#include "cbor_canbus_config_cbor_decode.h"
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

static bool decode_CborCanMessageConfig(zcbor_state_t *state, struct CborCanMessageConfig *result);
static bool decode_CborCanChannelConfig(zcbor_state_t *state, struct CborCanChannelConfig *result);
static bool decode_CborCanbusConfig(zcbor_state_t *state, struct CborCanbusConfig *result);


static bool decode_CborCanMessageConfig(
		zcbor_state_t *state, struct CborCanMessageConfig *result)
{
	zcbor_log("%s\r\n", __func__);

	bool res = (((zcbor_list_start_decode(state) && ((((zcbor_uint32_decode(state, (&(*result).frame_id))))
	&& ((zcbor_uint32_decode(state, (&(*result).send_interval_ms))))) || (zcbor_list_map_end_force_decode(state), false)) && zcbor_list_end_decode(state))));

	log_result(state, res, __func__);
	return res;
}

static bool decode_CborCanChannelConfig(
		zcbor_state_t *state, struct CborCanChannelConfig *result)
{
	zcbor_log("%s\r\n", __func__);

	bool res = (((zcbor_list_start_decode(state) && ((((zcbor_uint32_decode(state, (&(*result).type))))
	&& ((zcbor_uint32_decode(state, (&(*result).bus_channel))))
	&& ((zcbor_uint32_decode(state, (&(*result).bitrate))))
	&& ((zcbor_list_start_decode(state) && ((zcbor_multi_decode(0, 24, &(*result).CborCanMessageConfig_m_count, (zcbor_decoder_t *)decode_CborCanMessageConfig, state, (*&(*result).CborCanMessageConfig_m), sizeof(struct CborCanMessageConfig))) || (zcbor_list_map_end_force_decode(state), false)) && zcbor_list_end_decode(state)))) || (zcbor_list_map_end_force_decode(state), false)) && zcbor_list_end_decode(state))));

	if (false) {
		/* For testing that the types of the arguments are correct.
		 * A compiler error here means a bug in zcbor.
		 */
		decode_CborCanMessageConfig(state, (*&(*result).CborCanMessageConfig_m));
	}

	log_result(state, res, __func__);
	return res;
}

static bool decode_CborCanbusConfig(
		zcbor_state_t *state, struct CborCanbusConfig *result)
{
	zcbor_log("%s\r\n", __func__);

	bool res = (((zcbor_list_start_decode(state) && ((((zcbor_list_start_decode(state) && ((zcbor_multi_decode(0, 8, &(*result).CborCanChannelConfig_m_count, (zcbor_decoder_t *)decode_CborCanChannelConfig, state, (*&(*result).CborCanChannelConfig_m), sizeof(struct CborCanChannelConfig))) || (zcbor_list_map_end_force_decode(state), false)) && zcbor_list_end_decode(state)))
	&& ((zcbor_uint32_decode(state, (&(*result).json_config_checksum))))) || (zcbor_list_map_end_force_decode(state), false)) && zcbor_list_end_decode(state))));

	if (false) {
		/* For testing that the types of the arguments are correct.
		 * A compiler error here means a bug in zcbor.
		 */
		decode_CborCanChannelConfig(state, (*&(*result).CborCanChannelConfig_m));
	}

	log_result(state, res, __func__);
	return res;
}



int cbor_decode_CborCanbusConfig(
		const uint8_t *payload, size_t payload_len,
		struct CborCanbusConfig *result,
		size_t *payload_len_out)
{
	zcbor_state_t states[7];

	return zcbor_entry_function(payload, payload_len, (void *)result, payload_len_out, states,
		(zcbor_decoder_t *)decode_CborCanbusConfig, sizeof(states) / sizeof(zcbor_state_t), 1);
}
