#pragma once

#include <cstdint>
#include <cstdbool>
#include <cstddef>
#include <cstring>
#include "cbor_sensors_config.h"

int cbor_encode_CborSensorsConfig(
		uint8_t *payload, size_t payload_len,
		const struct CborSensorsConfig *input,
		size_t *payload_len_out);
