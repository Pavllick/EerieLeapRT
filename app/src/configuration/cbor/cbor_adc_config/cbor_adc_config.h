#pragma once

#include <memory_resource>
#include <cstdint>
#include <cstdbool>
#include <cstddef>
#include <vector>

struct CborAdcCalibrationDataMap_float32float {
	float float32float_key{};
	float float32float{};
};

struct CborAdcCalibrationDataMap {
	using allocator_type = std::pmr::polymorphic_allocator<>;

	std::pmr::vector<CborAdcCalibrationDataMap_float32float> float32float;

	CborAdcCalibrationDataMap(std::allocator_arg_t, allocator_type alloc)
        : float32float(alloc) {}

	CborAdcCalibrationDataMap(const CborAdcCalibrationDataMap&) = delete;
	CborAdcCalibrationDataMap& operator=(const CborAdcCalibrationDataMap&) noexcept = default;
	CborAdcCalibrationDataMap& operator=(CborAdcCalibrationDataMap&&) noexcept = default;
	CborAdcCalibrationDataMap(CborAdcCalibrationDataMap&&) noexcept = default;
	~CborAdcCalibrationDataMap() = default;

	CborAdcCalibrationDataMap(CborAdcCalibrationDataMap&& other, allocator_type alloc)
        : float32float(std::move(other.float32float), alloc) {}
};

struct CborAdcChannelConfig {
	using allocator_type = std::pmr::polymorphic_allocator<>;

	uint32_t interpolation_method{};
	CborAdcCalibrationDataMap calibration_table;
	bool calibration_table_present{};

	CborAdcChannelConfig(std::allocator_arg_t, allocator_type alloc)
        : calibration_table(std::allocator_arg, alloc) {}

    CborAdcChannelConfig(const CborAdcChannelConfig&) = delete;
	CborAdcChannelConfig& operator=(const CborAdcChannelConfig&) noexcept = default;
	CborAdcChannelConfig& operator=(CborAdcChannelConfig&&) noexcept = default;
	CborAdcChannelConfig(CborAdcChannelConfig&&) noexcept = default;
	~CborAdcChannelConfig() = default;

	CborAdcChannelConfig(CborAdcChannelConfig&& other, allocator_type alloc)
        : interpolation_method(other.interpolation_method),
		calibration_table(std::move(other.calibration_table), alloc),
		calibration_table_present(other.calibration_table_present) {}
};

struct CborAdcConfig {
	using allocator_type = std::pmr::polymorphic_allocator<>;

	uint32_t samples{};
	std::pmr::vector<CborAdcChannelConfig> CborAdcChannelConfig_m;
	uint32_t json_config_checksum{};

	CborAdcConfig(std::allocator_arg_t, allocator_type alloc)
        : CborAdcChannelConfig_m(alloc) {}

    CborAdcConfig(const CborAdcConfig&) = delete;
	CborAdcConfig& operator=(const CborAdcConfig&) noexcept = default;
	CborAdcConfig& operator=(CborAdcConfig&&) noexcept = default;
	CborAdcConfig(CborAdcConfig&&) noexcept = default;
	~CborAdcConfig() = default;

	CborAdcConfig(CborAdcConfig&& other, allocator_type alloc)
        : samples(other.samples),
		CborAdcChannelConfig_m(std::move(other.CborAdcChannelConfig_m), alloc),
		json_config_checksum(other.json_config_checksum) {}
};
