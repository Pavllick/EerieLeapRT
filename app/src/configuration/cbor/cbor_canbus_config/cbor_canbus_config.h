/*
 * Generated using zcbor version 0.9.1
 * https://github.com/NordicSemiconductor/zcbor
 * Generated with a --default-max-qty of 24
 */

#ifndef CBOR_CANBUS_CONFIG_H__
#define CBOR_CANBUS_CONFIG_H__

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>


#ifdef __cplusplus
extern "C" {
#endif

/** Which value for --default-max-qty this file was created with.
 *
 *  The define is used in the other generated file to do a build-time
 *  compatibility check.
 *
 *  See `zcbor --help` for more information about --default-max-qty
 */
#define DEFAULT_MAX_QTY 24

struct CborCanMessageConfig {
	uint32_t frame_id;
	uint32_t send_interval_ms;
};

struct CborCanChannelConfig {
	uint32_t type;
	uint32_t bus_channel;
	uint32_t bitrate;
	struct CborCanMessageConfig CborCanMessageConfig_m[24];
	size_t CborCanMessageConfig_m_count;
};

struct CborCanbusConfig {
	struct CborCanChannelConfig CborCanChannelConfig_m[8];
	size_t CborCanChannelConfig_m_count;
	uint32_t json_config_checksum;
};

#ifdef __cplusplus
}
#endif

#endif /* CBOR_CANBUS_CONFIG_H__ */
