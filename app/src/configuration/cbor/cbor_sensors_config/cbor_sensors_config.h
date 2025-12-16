#pragma once

#include <memory_resource>
#include <cstdint>
#include <cstdbool>
#include <cstddef>
#include <vector>

#include <zcbor_common.h>

struct CborSensorMetadataConfig {
	struct zcbor_string name{};
	struct zcbor_string unit{};
	struct zcbor_string description{};
};

struct CborSensorCalibrationDataMap_float32float {
	float float32float_key{};
	float float32float{};
};

struct CborSensorCalibrationDataMap {
	using allocator_type = std::pmr::polymorphic_allocator<>;

	std::pmr::vector<CborSensorCalibrationDataMap_float32float> float32float;

	CborSensorCalibrationDataMap(std::allocator_arg_t, allocator_type alloc)
        : float32float(alloc) {}

    CborSensorCalibrationDataMap(const CborSensorCalibrationDataMap&) = delete;
	CborSensorCalibrationDataMap& operator=(const CborSensorCalibrationDataMap&) noexcept = default;
	CborSensorCalibrationDataMap& operator=(CborSensorCalibrationDataMap&&) noexcept = default;
	CborSensorCalibrationDataMap(CborSensorCalibrationDataMap&&) noexcept = default;
	~CborSensorCalibrationDataMap() = default;

	CborSensorCalibrationDataMap(CborSensorCalibrationDataMap&& other, allocator_type alloc)
        : float32float(std::move(other.float32float), alloc) {}
};

struct CborSensorConfigurationConfig {
	using allocator_type = std::pmr::polymorphic_allocator<>;

	uint32_t type{};
	uint32_t sampling_rate_ms{};
	uint32_t interpolation_method{};
	uint32_t channel{};
	bool channel_present{};
	struct zcbor_string connection_string{};
	struct zcbor_string script_path{};
	struct CborSensorCalibrationDataMap calibration_table;
	bool calibration_table_present{};
	struct zcbor_string expression{};
	bool expression_present{};

	CborSensorConfigurationConfig(std::allocator_arg_t, allocator_type alloc)
        : calibration_table(std::allocator_arg, alloc) {}

    CborSensorConfigurationConfig(const CborSensorConfigurationConfig&) = delete;
	CborSensorConfigurationConfig& operator=(const CborSensorConfigurationConfig&) noexcept = default;
	CborSensorConfigurationConfig& operator=(CborSensorConfigurationConfig&&) noexcept = default;
	CborSensorConfigurationConfig(CborSensorConfigurationConfig&&) noexcept = default;
	~CborSensorConfigurationConfig() = default;

	CborSensorConfigurationConfig(CborSensorConfigurationConfig&& other, allocator_type alloc)
        : type(other.type),
		sampling_rate_ms(other.sampling_rate_ms),
		interpolation_method(other.interpolation_method),
		channel(other.channel),
		channel_present(other.channel_present),
		connection_string(other.connection_string),
		script_path(other.script_path),
		calibration_table(std::move(other.calibration_table), alloc),
		calibration_table_present(other.calibration_table_present),
		expression(other.expression),
		expression_present(other.expression_present) {}
};

struct CborSensorConfig {
	using allocator_type = std::pmr::polymorphic_allocator<>;

	struct zcbor_string id{};
	struct CborSensorMetadataConfig metadata{};
	struct CborSensorConfigurationConfig configuration;

	CborSensorConfig(std::allocator_arg_t, allocator_type alloc)
        : configuration(std::allocator_arg, alloc) {}

    CborSensorConfig(const CborSensorConfig&) = delete;
	CborSensorConfig& operator=(const CborSensorConfig&) noexcept = default;
	CborSensorConfig& operator=(CborSensorConfig&&) noexcept = default;
	CborSensorConfig(CborSensorConfig&&) noexcept = default;
	~CborSensorConfig() = default;

	CborSensorConfig(CborSensorConfig&& other, allocator_type alloc)
        : id(other.id),
		metadata(other.metadata),
		configuration(std::move(other.configuration), alloc) {}
};

struct CborSensorsConfig {
	using allocator_type = std::pmr::polymorphic_allocator<>;

	std::pmr::vector<CborSensorConfig> CborSensorConfig_m;
	uint32_t json_config_checksum{};

	CborSensorsConfig(std::allocator_arg_t, allocator_type alloc)
        : CborSensorConfig_m(alloc) {}

    CborSensorsConfig(const CborSensorsConfig&) = delete;
	CborSensorsConfig& operator=(const CborSensorsConfig&) noexcept = default;
	CborSensorsConfig& operator=(CborSensorsConfig&&) noexcept = default;
	CborSensorsConfig(CborSensorsConfig&&) noexcept = default;
	~CborSensorsConfig() = default;

	CborSensorsConfig(CborSensorsConfig&& other, allocator_type alloc)
        : CborSensorConfig_m(std::move(other.CborSensorConfig_m), alloc),
		json_config_checksum(other.json_config_checksum) {}
};
