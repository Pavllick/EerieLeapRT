/*
 * Generated using zcbor version 0.9.1
 * https://github.com/NordicSemiconductor/zcbor
 * Generated with a --default-max-qty of 24
 */

#ifndef SYSTEM_CONFIG_H__
#define SYSTEM_CONFIG_H__

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

struct ComUserConfig {
	uint64_t device_id;
	uint32_t server_id;
};

struct SystemConfig {
	uint64_t device_id;
	uint32_t hw_version;
	uint32_t sw_version;
	uint32_t build_number;
	struct ComUserConfig ComUserConfig_m[24];
	size_t ComUserConfig_m_count;
};

#ifdef __cplusplus
}
#endif

#endif /* SYSTEM_CONFIG_H__ */
