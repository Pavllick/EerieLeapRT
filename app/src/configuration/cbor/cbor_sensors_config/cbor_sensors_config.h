#pragma once

#include <cstdint>
#include <cstdbool>
#include <cstddef>
#include <vector>

#include <zcbor_common.h>

#include "utilities/memory/heap_allocator.h"

using namespace eerie_leap::utilities::memory;

struct CborSensorMetadataConfig {
	struct zcbor_string name;
	struct zcbor_string unit;
	struct zcbor_string description;
};

struct CborSensorCalibrationDataMap_float32float {
	float float32float_key;
	float float32float;
};

struct CborSensorCalibrationDataMap {
	std::vector<CborSensorCalibrationDataMap_float32float, HeapAllocator<CborSensorCalibrationDataMap_float32float>> float32float;
};

struct CborSensorConfigurationConfig {
	uint32_t type;
	uint32_t sampling_rate_ms;
	uint32_t interpolation_method;
	uint32_t channel;
	bool channel_present;
	struct zcbor_string connection_string;
	struct zcbor_string script_path;
	struct CborSensorCalibrationDataMap calibration_table;
	bool calibration_table_present;
	struct zcbor_string expression;
	bool expression_present;
};

struct CborSensorConfig {
	struct zcbor_string id;
	struct CborSensorMetadataConfig metadata;
	struct CborSensorConfigurationConfig configuration;
};

struct CborSensorsConfig {
	std::vector<CborSensorConfig, HeapAllocator<CborSensorConfig>> CborSensorConfig_m;
	uint32_t json_config_checksum;
};
