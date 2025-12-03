#pragma once

#include <cstdint>
#include <cstdbool>
#include <cstddef>
#include <vector>

#include <zcbor_common.h>

#include "utilities/memory/heap_allocator.h"

using namespace eerie_leap::utilities::memory;

struct CborCanMessageConfig {
	uint32_t frame_id;
	uint32_t send_interval_ms;
	struct zcbor_string script_path;
};

struct CborCanChannelConfig {
	uint32_t type;
	bool is_extended_id;
	uint32_t bus_channel;
	uint32_t bitrate;
	uint32_t data_bitrate;
	struct zcbor_string dbc_file_path;
	std::vector<CborCanMessageConfig, HeapAllocator<CborCanMessageConfig>> CborCanMessageConfig_m;
};

struct CborCanbusConfig {
	std::vector<CborCanChannelConfig, HeapAllocator<CborCanChannelConfig>> CborCanChannelConfig_m;
	uint32_t json_config_checksum;
};
