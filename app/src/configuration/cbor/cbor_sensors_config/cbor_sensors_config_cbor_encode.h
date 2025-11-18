/*
 * Generated using zcbor version 0.9.1
 * https://github.com/NordicSemiconductor/zcbor
 * Generated with a --default-max-qty of 24
 */

#ifndef CBOR_SENSORS_CONFIG_CBOR_ENCODE_H__
#define CBOR_SENSORS_CONFIG_CBOR_ENCODE_H__

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include "cbor_sensors_config.h"

#ifdef __cplusplus
extern "C" {
#endif

#if DEFAULT_MAX_QTY != 24
#error "The type file was generated with a different default_max_qty than this file"
#endif


int cbor_encode_CborSensorsConfig(
		uint8_t *payload, size_t payload_len,
		const struct CborSensorsConfig *input,
		size_t *payload_len_out);


#ifdef __cplusplus
}
#endif

#endif /* CBOR_SENSORS_CONFIG_CBOR_ENCODE_H__ */
