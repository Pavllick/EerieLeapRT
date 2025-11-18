#pragma once

#include <cstdint>
#include <cstdbool>
#include <cstddef>
#include <cstring>
#include "cbor_sensors_config.h"

int cbor_decode_CborSensorsConfig(
		const uint8_t *payload, size_t payload_len,
		struct CborSensorsConfig *result,
		size_t *payload_len_out);
