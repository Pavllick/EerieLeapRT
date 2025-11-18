#pragma once

#include <cstdint>
#include <cstdbool>
#include <cstddef>
#include <cstring>
#include "cbor_logging_config.h"

int cbor_decode_CborLoggingConfig(
		const uint8_t *payload, size_t payload_len,
		struct CborLoggingConfig *result,
		size_t *payload_len_out);
