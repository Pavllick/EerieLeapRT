#pragma once

#include <cstdint>
#include <cstdbool>
#include <cstddef>
#include <cstring>
#include "cbor_adc_config.h"

int cbor_encode_CborAdcConfig(
		uint8_t *payload, size_t payload_len,
		const struct CborAdcConfig *input,
		size_t *payload_len_out);
