#pragma once

#include <cstdint>
#include <chrono>

#include <zephyr/sys/byteorder.h>
#include <zephyr/sys/crc.h>

#include "utilities/constants.h"
#include "utilities/time/time_helpers.hpp"

namespace eerie_leap::domain::logging_domain::models {

using namespace std::chrono;
using namespace eerie_leap::utilities::constants::logging;
using namespace eerie_leap::utilities::time;

struct LogMetadata {
    uint32_t file_type;
    uint32_t version;
    uint64_t start_timestamp;
    uint64_t reserved;
    uint32_t crc;

    static LogMetadata Create(const system_clock::time_point& tp) {
        LogMetadata metadata {
            .file_type = LOG_METADATA_FILE_TYPE,
            .version = CONFIG_LOG_METADATA_FILE_VERSION,
            .start_timestamp = TimeHelpers::ToUint64(tp)
        };
        metadata.crc = sys_cpu_to_le32(
            crc32_ieee((uint8_t*)(&metadata), sizeof(metadata) - sizeof(metadata.crc)));

        return metadata;
    }
} __attribute__((packed, aligned(1))); // Ensure no padding

} // namespace eerie_leap::domain::logging_domain::models
