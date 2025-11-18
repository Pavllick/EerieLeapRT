#pragma once

#include <cstdint>
#include <cstdbool>
#include <cstddef>
#include <cstring>
#include "cbor_logging_config.h"

int cbor_encode_CborLoggingConfig(
		uint8_t *payload, size_t payload_len,
		const struct CborLoggingConfig *input,
		size_t *payload_len_out);
