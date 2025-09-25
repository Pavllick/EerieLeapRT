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

struct LogDataHeader {
    uint32_t file_type;
    uint32_t version;
    uint32_t log_metadata_file_version;
    uint64_t start_timestamp;
    uint64_t reserved;
    uint32_t crc;

    static LogDataHeader Create(const system_clock::time_point& tp, uint32_t log_metadata_file_version) {
        LogDataHeader header {
            .file_type = LOG_DATA_FILE_TYPE,
            .version = CONFIG_EERIE_LEAP_LOG_DATA_FILE_VERSION,
            .log_metadata_file_version = log_metadata_file_version,
            .start_timestamp = TimeHelpers::ToUint64(tp)
        };
        header.crc = crc32_ieee((uint8_t*)(&header), sizeof(header) - sizeof(header.crc));

        return header;
    }
} __attribute__((packed, aligned(1))); // Ensure no padding

} // namespace eerie_leap::domain::logging_domain::models
