#include <cstdint>
#include <cstdbool>
#include <cstddef>
#include <cstring>
#include "zcbor_decode.h"
#include "cbor_canbus_config_cbor_decode.h"
#include "zcbor_print.h"

#define log_result(state, result, func) do { \
	if (!result) { \
		zcbor_trace_file(state); \
		zcbor_log("%s error: %s\r\n", func, zcbor_error_str(zcbor_peek_error(state))); \
	} else { \
		zcbor_log("%s success\r\n", func); \
	} \
} while(0)

static bool decode_CborCanSignalConfig(zcbor_state_t *state, struct CborCanSignalConfig *result);
static bool decode_CborCanMessageConfig(zcbor_state_t *state, struct CborCanMessageConfig *result);
static bool decode_CborCanChannelConfig(zcbor_state_t *state, struct CborCanChannelConfig *result);
static bool decode_CborCanbusConfig(zcbor_state_t *state, struct CborCanbusConfig *result);


static bool decode_CborCanSignalConfig(
		zcbor_state_t *state, struct CborCanSignalConfig *result)
{
	zcbor_log("%s\r\n", __func__);

	bool res = (((zcbor_list_start_decode(state) && ((((zcbor_uint32_decode(state, (&(*result).start_bit))))
	&& ((zcbor_uint32_decode(state, (&(*result).size_bits))))
	&& ((zcbor_float32_decode(state, (&(*result).factor))))
	&& ((zcbor_float32_decode(state, (&(*result).offset))))
	&& ((zcbor_tstr_decode(state, (&(*result).name))))
	&& ((zcbor_tstr_decode(state, (&(*result).unit))))) || (zcbor_list_map_end_force_decode(state), false)) && zcbor_list_end_decode(state))));

	log_result(state, res, __func__);
	return res;
}

static bool decode_CborCanMessageConfig(
		zcbor_state_t *state, struct CborCanMessageConfig *result)
{
	zcbor_log("%s\r\n", __func__);

    if (!zcbor_list_start_decode(state)) {
        return false;
    }

    if (!zcbor_uint32_decode(state, &result->frame_id)) {
        zcbor_list_end_decode(state);
        return false;
    }

    if (!zcbor_uint32_decode(state, &result->send_interval_ms)) {
        zcbor_list_end_decode(state);
        return false;
    }

    if (!zcbor_tstr_decode(state, &result->script_path)) {
        zcbor_list_end_decode(state);
        return false;
    }

    if (!zcbor_tstr_decode(state, &result->name)) {
        zcbor_list_end_decode(state);
        return false;
    }

    if (!zcbor_uint32_decode(state, &result->message_size)) {
        zcbor_list_end_decode(state);
        return false;
    }

    if (!zcbor_list_start_decode(state)) {
        zcbor_list_end_decode(state);
        return false;
    }

    while (!zcbor_array_at_end(state)) {
        result->CborCanSignalConfig_m.emplace_back();
        if (!decode_CborCanSignalConfig(state, &result->CborCanSignalConfig_m.back())) {
            result->CborCanSignalConfig_m.pop_back();
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

    if (!zcbor_list_end_decode(state)) {
        return false;
    }

	log_result(state, true, __func__);
	return true;
}

static bool decode_CborCanChannelConfig(
        zcbor_state_t *state, struct CborCanChannelConfig *result)
{
    zcbor_log("%s\r\n", __func__);

    if (!zcbor_list_start_decode(state)) {
        return false;
    }

    if (!zcbor_uint32_decode(state, &result->type)) {
        zcbor_list_end_decode(state);
        return false;
    }

    if (!zcbor_bool_decode(state, &result->is_extended_id)) {
        zcbor_list_end_decode(state);
        return false;
    }

    if (!zcbor_uint32_decode(state, &result->bus_channel)) {
        zcbor_list_end_decode(state);
        return false;
    }

    if (!zcbor_uint32_decode(state, &result->bitrate)) {
        zcbor_list_end_decode(state);
        return false;
    }

    if (!zcbor_uint32_decode(state, &result->data_bitrate)) {
        zcbor_list_end_decode(state);
        return false;
    }

    if (!zcbor_tstr_decode(state, &result->dbc_file_path)) {
        zcbor_list_end_decode(state);
        return false;
    }

    if (!zcbor_list_start_decode(state)) {
        zcbor_list_end_decode(state);
        return false;
    }

    while (!zcbor_array_at_end(state)) {
        result->CborCanMessageConfig_m.emplace_back();
        if (!decode_CborCanMessageConfig(state, &result->CborCanMessageConfig_m.back())) {
            result->CborCanMessageConfig_m.pop_back();
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

    if (!zcbor_list_end_decode(state)) {
        return false;
    }

    log_result(state, true, __func__);
    return true;
}

static bool decode_CborCanbusConfig(
        zcbor_state_t *state, struct CborCanbusConfig *result)
{
    zcbor_log("%s\r\n", __func__);

    if (!zcbor_list_start_decode(state)) {
        return false;
    }

    if (!zcbor_list_start_decode(state)) {
        zcbor_list_end_decode(state);
        return false;
    }

    while (!zcbor_array_at_end(state)) {
        result->CborCanChannelConfig_m.emplace_back();
        if (!decode_CborCanChannelConfig(state, &result->CborCanChannelConfig_m.back())) {
            result->CborCanChannelConfig_m.pop_back();
            zcbor_list_map_end_force_decode(state);
            zcbor_list_end_decode(state);
            zcbor_list_end_decode(state);
            return false;
        }
    }

    if (!zcbor_list_end_decode(state)) {
        return false;
    }

    if (!zcbor_uint32_decode(state, &result->json_config_checksum)) {
        return false;
    }

    if (!zcbor_list_end_decode(state)) {
        return false;
    }

    log_result(state, true, __func__);
    return true;
}



int cbor_decode_CborCanbusConfig(
		const uint8_t *payload, size_t payload_len,
		struct CborCanbusConfig *result,
		size_t *payload_len_out)
{
	zcbor_state_t states[9];

	return zcbor_entry_function(payload, payload_len, (void *)result, payload_len_out, states,
		(zcbor_decoder_t *)decode_CborCanbusConfig, sizeof(states) / sizeof(zcbor_state_t), 1);
}
