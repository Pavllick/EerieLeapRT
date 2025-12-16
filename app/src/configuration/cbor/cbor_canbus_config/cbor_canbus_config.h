#pragma once

#include <memory_resource>
#include <cstdint>
#include <cstdbool>
#include <cstddef>
#include <vector>

#include <zcbor_common.h>

struct CborCanSignalConfig {
	uint32_t start_bit{};
	uint32_t size_bits{};
	float factor{};
	float offset{};
	struct zcbor_string name{};
	struct zcbor_string unit{};
};

struct CborCanMessageConfig {
	using allocator_type = std::pmr::polymorphic_allocator<>;

	uint32_t frame_id{};
	uint32_t send_interval_ms{};
	struct zcbor_string script_path{};

	struct zcbor_string name{};
	uint32_t message_size{};
	std::pmr::vector<CborCanSignalConfig> CborCanSignalConfig_m;

	CborCanMessageConfig(std::allocator_arg_t, allocator_type alloc)
        : CborCanSignalConfig_m(alloc) {}

    CborCanMessageConfig(const CborCanMessageConfig&) = delete;
	CborCanMessageConfig& operator=(const CborCanMessageConfig&) noexcept = default;
	CborCanMessageConfig& operator=(CborCanMessageConfig&&) noexcept = default;
	CborCanMessageConfig(CborCanMessageConfig&&) noexcept = default;
	~CborCanMessageConfig() = default;

	CborCanMessageConfig(CborCanMessageConfig&& other, allocator_type alloc)
        : frame_id(other.frame_id),
		send_interval_ms(other.send_interval_ms),
		script_path(other.script_path),
		name(other.name),
		message_size(other.message_size),
		CborCanSignalConfig_m(std::move(other.CborCanSignalConfig_m), alloc) {}
};

struct CborCanChannelConfig {
	using allocator_type = std::pmr::polymorphic_allocator<>;

	uint32_t type{};
	bool is_extended_id{};
	uint32_t bus_channel{};
	uint32_t bitrate{};
	uint32_t data_bitrate{};
	struct zcbor_string dbc_file_path{};
	std::pmr::vector<CborCanMessageConfig> CborCanMessageConfig_m;

	CborCanChannelConfig(std::allocator_arg_t, allocator_type alloc)
        : CborCanMessageConfig_m(alloc) {}

    CborCanChannelConfig(const CborCanChannelConfig&) = delete;
	CborCanChannelConfig& operator=(const CborCanChannelConfig&) noexcept = default;
	CborCanChannelConfig& operator=(CborCanChannelConfig&&) noexcept = default;
	CborCanChannelConfig(CborCanChannelConfig&&) noexcept = default;
	~CborCanChannelConfig() = default;

	CborCanChannelConfig(CborCanChannelConfig&& other, allocator_type alloc)
        : type(other.type),
		is_extended_id(other.is_extended_id),
		bus_channel(other.bus_channel),
		bitrate(other.bitrate),
		data_bitrate(other.data_bitrate),
		dbc_file_path(other.dbc_file_path),
		CborCanMessageConfig_m(std::move(other.CborCanMessageConfig_m), alloc) {}
};

struct CborCanbusConfig {
	using allocator_type = std::pmr::polymorphic_allocator<>;

	std::pmr::vector<CborCanChannelConfig> CborCanChannelConfig_m;
	uint32_t json_config_checksum{};

	CborCanbusConfig(std::allocator_arg_t, allocator_type alloc)
        : CborCanChannelConfig_m(alloc) {}

    CborCanbusConfig(const CborCanbusConfig&) = delete;
	CborCanbusConfig& operator=(const CborCanbusConfig&) noexcept = default;
	CborCanbusConfig& operator=(CborCanbusConfig&&) noexcept = default;
	CborCanbusConfig(CborCanbusConfig&&) noexcept = default;
	~CborCanbusConfig() = default;

	CborCanbusConfig(CborCanbusConfig&& other, allocator_type alloc)
        : CborCanChannelConfig_m(std::move(other.CborCanChannelConfig_m), alloc),
		json_config_checksum(other.json_config_checksum) {}
};
