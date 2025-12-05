#pragma once

#include <cstdint>
#include <cstdbool>
#include <cstddef>
#include <vector>

#include <zcbor_common.h>

#include "utilities/memory/heap_allocator.h"

using namespace eerie_leap::utilities::memory;

struct CborCanSignalConfig {
	uint32_t start_bit;
	uint32_t size_bits;
	float factor;
	float offset;
	struct zcbor_string name;
	struct zcbor_string unit;
};

struct CborCanMessageConfig {
	uint32_t frame_id;
	uint32_t send_interval_ms;
	struct zcbor_string script_path;

	struct zcbor_string name;
	uint32_t message_size;
	std::vector<CborCanSignalConfig, HeapAllocator<CborCanSignalConfig>> CborCanSignalConfig_m;
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
