#pragma once

#include <cstdint>
#include <cstdbool>
#include <cstddef>
#include <vector>

#include "utilities/memory/heap_allocator.h"

using namespace eerie_leap::utilities::memory;

struct CborSensorLoggingConfig {
	uint32_t sensor_id_hash;
	bool is_enabled;
	bool log_raw_value;
	bool log_only_new_data;
};

struct CborLoggingConfig {
	uint32_t logging_interval_ms;
	uint32_t max_log_size_mb;
	std::vector<CborSensorLoggingConfig, HeapAllocator<CborSensorLoggingConfig>> CborSensorLoggingConfig_m;
	uint32_t json_config_checksum;
};
