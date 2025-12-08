#include <cstdint>
#include <cstdbool>
#include <cstddef>
#include <cstring>
#include "zcbor_encode.h"
#include "cbor_canbus_config_cbor_encode.h"
#include "zcbor_print.h"

#define log_result(state, result, func) do { \
	if (!result) { \
		zcbor_trace_file(state); \
		zcbor_log("%s error: %s\r\n", func, zcbor_error_str(zcbor_peek_error(state))); \
	} else { \
		zcbor_log("%s success\r\n", func); \
	} \
} while(0)

static bool encode_CborCanSignalConfig(zcbor_state_t *state, const struct CborCanSignalConfig *input);
static bool encode_CborCanMessageConfig(zcbor_state_t *state, const struct CborCanMessageConfig *input);
static bool encode_CborCanChannelConfig(zcbor_state_t *state, const struct CborCanChannelConfig *input);
static bool encode_CborCanbusConfig(zcbor_state_t *state, const struct CborCanbusConfig *input);


static bool encode_CborCanSignalConfig(
		zcbor_state_t *state, const struct CborCanSignalConfig *input)
{
	zcbor_log("%s\r\n", __func__);

	bool res = (((zcbor_list_start_encode(state, 6) && ((((zcbor_uint32_encode(state, (&(*input).start_bit))))
	&& ((zcbor_uint32_encode(state, (&(*input).size_bits))))
	&& ((zcbor_float32_encode(state, (&(*input).factor))))
	&& ((zcbor_float32_encode(state, (&(*input).offset))))
	&& ((zcbor_tstr_encode(state, (&(*input).name))))
	&& ((zcbor_tstr_encode(state, (&(*input).unit))))) || (zcbor_list_map_end_force_encode(state), false)) && zcbor_list_end_encode(state, 6))));

	log_result(state, res, __func__);
	return res;
}

static bool encode_CborCanMessageConfig(
		zcbor_state_t *state, const struct CborCanMessageConfig *input)
{
	zcbor_log("%s\r\n", __func__);

	size_t CborCanSignalConfig_m_count = input->CborCanSignalConfig_m.size();

	bool res = (((zcbor_list_start_encode(state, 6) && ((((zcbor_uint32_encode(state, (&(*input).frame_id))))
	&& ((zcbor_uint32_encode(state, (&(*input).send_interval_ms))))
	&& ((zcbor_tstr_encode(state, (&(*input).script_path))))
	&& ((zcbor_tstr_encode(state, (&(*input).name))))
	&& ((zcbor_uint32_encode(state, (&(*input).message_size))))
	&& ((zcbor_list_start_encode(state, CborCanSignalConfig_m_count) && ((zcbor_multi_encode_minmax(0, CborCanSignalConfig_m_count, &CborCanSignalConfig_m_count, (zcbor_encoder_t *)encode_CborCanSignalConfig, state, input->CborCanSignalConfig_m.data(), sizeof(struct CborCanSignalConfig))) || (zcbor_list_map_end_force_encode(state), false)) && zcbor_list_end_encode(state, CborCanSignalConfig_m_count)))) || (zcbor_list_map_end_force_encode(state), false)) && zcbor_list_end_encode(state, 6))));

	log_result(state, res, __func__);
	return res;
}

static bool encode_CborCanChannelConfig(
		zcbor_state_t *state, const struct CborCanChannelConfig *input)
{
	zcbor_log("%s\r\n", __func__);

	size_t CborCanMessageConfig_m_count = input->CborCanMessageConfig_m.size();

	bool res = (((zcbor_list_start_encode(state, 7) && ((((zcbor_uint32_encode(state, (&(*input).type))))
	&& ((zcbor_bool_encode(state, (&(*input).is_extended_id))))
	&& ((zcbor_uint32_encode(state, (&(*input).bus_channel))))
	&& ((zcbor_uint32_encode(state, (&(*input).bitrate))))
	&& ((zcbor_uint32_encode(state, (&(*input).data_bitrate))))
	&& ((zcbor_tstr_encode(state, (&(*input).dbc_file_path))))
	&& ((zcbor_list_start_encode(state, CborCanMessageConfig_m_count) && ((zcbor_multi_encode_minmax(0, CborCanMessageConfig_m_count, &CborCanMessageConfig_m_count, (zcbor_encoder_t *)encode_CborCanMessageConfig, state, input->CborCanMessageConfig_m.data(), sizeof(struct CborCanMessageConfig))) || (zcbor_list_map_end_force_encode(state), false)) && zcbor_list_end_encode(state, CborCanMessageConfig_m_count)))) || (zcbor_list_map_end_force_encode(state), false)) && zcbor_list_end_encode(state, 7))));

	log_result(state, res, __func__);
	return res;
}

static bool encode_CborCanbusConfig(
		zcbor_state_t *state, const struct CborCanbusConfig *input)
{
	zcbor_log("%s\r\n", __func__);

	size_t CborCanChannelConfig_m_count = input->CborCanChannelConfig_m.size();

	bool res = (((zcbor_list_start_encode(state, 2) && ((((zcbor_list_start_encode(state, CborCanChannelConfig_m_count) && ((zcbor_multi_encode_minmax(0, CborCanChannelConfig_m_count, &CborCanChannelConfig_m_count, (zcbor_encoder_t *)encode_CborCanChannelConfig, state, input->CborCanChannelConfig_m.data(), sizeof(struct CborCanChannelConfig))) || (zcbor_list_map_end_force_encode(state), false)) && zcbor_list_end_encode(state, CborCanChannelConfig_m_count)))
	&& ((zcbor_uint32_encode(state, (&(*input).json_config_checksum))))) || (zcbor_list_map_end_force_encode(state), false)) && zcbor_list_end_encode(state, 2))));

	log_result(state, res, __func__);
	return res;
}



int cbor_encode_CborCanbusConfig(
		uint8_t *payload, size_t payload_len,
		const struct CborCanbusConfig *input,
		size_t *payload_len_out)
{
	zcbor_state_t states[9];

	return zcbor_entry_function(payload, payload_len, (void *)input, payload_len_out, states,
		(zcbor_decoder_t *)encode_CborCanbusConfig, sizeof(states) / sizeof(zcbor_state_t), 1);
}
