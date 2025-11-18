#pragma once

#include <cstdint>
#include <cstdbool>
#include <cstddef>
#include <cstring>
#include "cbor_adc_config.h"

int cbor_decode_CborAdcConfig(
		const uint8_t *payload, size_t payload_len,
		struct CborAdcConfig *result,
		size_t *payload_len_out);
