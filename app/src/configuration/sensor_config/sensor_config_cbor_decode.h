/*
 * Generated using zcbor version 0.9.1
 * https://github.com/NordicSemiconductor/zcbor
 * Generated with a --default-max-qty of 40
 */

#ifndef SENSOR_CONFIG_CBOR_DECODE_H__
#define SENSOR_CONFIG_CBOR_DECODE_H__

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include "sensor_config.h"

#ifdef __cplusplus
extern "C" {
#endif

#if DEFAULT_MAX_QTY != 40
#error "The type file was generated with a different default_max_qty than this file"
#endif


int cbor_decode_SensorsConfig(
		const uint8_t *payload, size_t payload_len,
		struct SensorsConfig *result,
		size_t *payload_len_out);


#ifdef __cplusplus
}
#endif

#endif /* SENSOR_CONFIG_CBOR_DECODE_H__ */
