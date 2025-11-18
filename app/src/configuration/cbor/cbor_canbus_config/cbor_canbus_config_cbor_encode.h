#pragma once

#include <cstdint>
#include <cstdbool>
#include <cstddef>
#include <cstring>
#include "cbor_canbus_config.h"

int cbor_encode_CborCanbusConfig(
		uint8_t *payload, size_t payload_len,
		const struct CborCanbusConfig *input,
		size_t *payload_len_out);
