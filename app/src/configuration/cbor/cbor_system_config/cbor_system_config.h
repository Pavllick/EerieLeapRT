/*
 * Generated using zcbor version 0.9.1
 * https://github.com/NordicSemiconductor/zcbor
 * Generated with a --default-max-qty of 24
 */

#ifndef CBOR_SYSTEM_CONFIG_H__
#define CBOR_SYSTEM_CONFIG_H__

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

struct CborComUserConfig {
	uint64_t device_id;
	uint32_t server_id;
};

struct CborCanbusConfig {
	uint32_t bus_channel;
	uint32_t bitrate;
};

struct CborSystemConfig {
	uint64_t device_id;
	uint32_t hw_version;
	uint32_t sw_version;
	uint32_t build_number;
	uint32_t com_user_refresh_rate_ms;
	struct CborComUserConfig CborComUserConfig_m[8];
	size_t CborComUserConfig_m_count;
	struct CborCanbusConfig CborCanbusConfig_m[8];
	size_t CborCanbusConfig_m_count;
	uint32_t json_config_checksum;
};

#ifdef __cplusplus
}
#endif

#endif /* CBOR_SYSTEM_CONFIG_H__ */
