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
#include "canbus_config_cbor_encode.h"
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

static bool encode_CborCanMessageConfig(zcbor_state_t *state, const struct CborCanMessageConfig *input);
static bool encode_CborCanChannelConfig(zcbor_state_t *state, const struct CborCanChannelConfig *input);
static bool encode_CborCanbusConfig(zcbor_state_t *state, const struct CborCanbusConfig *input);


static bool encode_CborCanMessageConfig(
		zcbor_state_t *state, const struct CborCanMessageConfig *input)
{
	zcbor_log("%s\r\n", __func__);

	bool res = (((zcbor_list_start_encode(state, 2) && ((((zcbor_uint32_encode(state, (&(*input).frame_id))))
	&& ((zcbor_uint32_encode(state, (&(*input).send_interval_ms))))) || (zcbor_list_map_end_force_encode(state), false)) && zcbor_list_end_encode(state, 2))));

	log_result(state, res, __func__);
	return res;
}

static bool encode_CborCanChannelConfig(
		zcbor_state_t *state, const struct CborCanChannelConfig *input)
{
	zcbor_log("%s\r\n", __func__);

	bool res = (((zcbor_list_start_encode(state, 4) && ((((zcbor_uint32_encode(state, (&(*input).type))))
	&& ((zcbor_uint32_encode(state, (&(*input).bus_channel))))
	&& ((zcbor_uint32_encode(state, (&(*input).bitrate))))
	&& ((zcbor_list_start_encode(state, 24) && ((zcbor_multi_encode_minmax(0, 24, &(*input).CborCanMessageConfig_m_count, (zcbor_encoder_t *)encode_CborCanMessageConfig, state, (*&(*input).CborCanMessageConfig_m), sizeof(struct CborCanMessageConfig))) || (zcbor_list_map_end_force_encode(state), false)) && zcbor_list_end_encode(state, 24)))) || (zcbor_list_map_end_force_encode(state), false)) && zcbor_list_end_encode(state, 4))));

	log_result(state, res, __func__);
	return res;
}

static bool encode_CborCanbusConfig(
		zcbor_state_t *state, const struct CborCanbusConfig *input)
{
	zcbor_log("%s\r\n", __func__);

	bool res = (((zcbor_list_start_encode(state, 2) && ((((zcbor_list_start_encode(state, 8) && ((zcbor_multi_encode_minmax(0, 8, &(*input).CborCanChannelConfig_m_count, (zcbor_encoder_t *)encode_CborCanChannelConfig, state, (*&(*input).CborCanChannelConfig_m), sizeof(struct CborCanChannelConfig))) || (zcbor_list_map_end_force_encode(state), false)) && zcbor_list_end_encode(state, 8)))
	&& ((zcbor_uint32_encode(state, (&(*input).json_config_checksum))))) || (zcbor_list_map_end_force_encode(state), false)) && zcbor_list_end_encode(state, 2))));

	log_result(state, res, __func__);
	return res;
}



int cbor_encode_CborCanbusConfig(
		uint8_t *payload, size_t payload_len,
		const struct CborCanbusConfig *input,
		size_t *payload_len_out)
{
	zcbor_state_t states[7];

	return zcbor_entry_function(payload, payload_len, (void *)input, payload_len_out, states,
		(zcbor_decoder_t *)encode_CborCanbusConfig, sizeof(states) / sizeof(zcbor_state_t), 1);
}
