#pragma once

#include <cstdint>
#include <cstdbool>
#include <cstddef>
#include <vector>

#include <zcbor_common.h>

struct CborCanMessageConfig {
	uint32_t frame_id;
	uint32_t send_interval_ms;
};

struct CborCanChannelConfig {
	uint32_t type;
	uint32_t bus_channel;
	uint32_t bitrate;
	struct zcbor_string dbc_file_path;
	std::vector<CborCanMessageConfig> CborCanMessageConfig_m;
};

struct CborCanbusConfig {
	std::vector<CborCanChannelConfig> CborCanChannelConfig_m;
	uint32_t json_config_checksum;
};
