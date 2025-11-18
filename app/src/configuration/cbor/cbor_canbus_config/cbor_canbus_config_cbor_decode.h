#pragma once

#include <cstdint>
#include <cstdbool>
#include <cstddef>
#include <cstring>
#include "cbor_canbus_config.h"

int cbor_decode_CborCanbusConfig(
		const uint8_t *payload, size_t payload_len,
		struct CborCanbusConfig *result,
		size_t *payload_len_out);
