#pragma once

#include <cstdint>
#include <cstdbool>
#include <cstddef>
#include <vector>

#include "utilities/memory/heap_allocator.h"

using namespace eerie_leap::utilities::memory;

struct CborAdcCalibrationDataMap_float32float {
	float float32float_key;
	float float32float;
};

struct CborAdcCalibrationDataMap {
	std::vector<CborAdcCalibrationDataMap_float32float> float32float;
};

struct CborAdcChannelConfig {
	uint32_t interpolation_method;
	struct CborAdcCalibrationDataMap calibration_table;
	bool calibration_table_present;
};

struct CborAdcConfig {
	uint32_t samples;
	std::vector<CborAdcChannelConfig, HeapAllocator<CborAdcChannelConfig>> CborAdcChannelConfig_m;
	uint32_t json_config_checksum;
};
