#pragma once

#include <memory_resource>
#include <cstdint>
#include <cstdbool>
#include <cstddef>
#include <vector>

struct CborSensorLoggingConfig {
	uint32_t sensor_id_hash;
	bool is_enabled;
	bool log_raw_value;
	bool log_only_new_data;
};

struct CborLoggingConfig {
	using allocator_type = std::pmr::polymorphic_allocator<>;

	uint32_t logging_interval_ms;
	uint32_t max_log_size_mb;
	std::pmr::vector<CborSensorLoggingConfig> CborSensorLoggingConfig_m;
	uint32_t json_config_checksum;

	CborLoggingConfig(std::allocator_arg_t, allocator_type alloc)
        : CborSensorLoggingConfig_m(alloc) {}

    CborLoggingConfig(const CborLoggingConfig&) = delete;
	CborLoggingConfig& operator=(const CborLoggingConfig&) noexcept = default;
	CborLoggingConfig& operator=(CborLoggingConfig&&) noexcept = default;
	CborLoggingConfig(CborLoggingConfig&&) noexcept = default;
	~CborLoggingConfig() = default;

	CborLoggingConfig(CborLoggingConfig&& other, allocator_type alloc)
        : logging_interval_ms(other.logging_interval_ms),
		max_log_size_mb(other.max_log_size_mb),
		CborSensorLoggingConfig_m(std::move(other.CborSensorLoggingConfig_m), alloc),
		json_config_checksum(other.json_config_checksum) {}
};
