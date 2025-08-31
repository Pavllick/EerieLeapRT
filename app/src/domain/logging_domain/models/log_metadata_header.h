#pragma once

#include <cstdint>

#include "utilities/constants.h"

namespace eerie_leap::domain::logging_domain::models {

using namespace eerie_leap::utilities::constants::logging;

struct LogMetadataHeader {
    uint32_t file_type;
    uint32_t version;
    uint64_t reserved;

    static LogMetadataHeader Create() {
        LogMetadataHeader header {
            .file_type = LOG_METADATA_FILE_TYPE,
            .version = CONFIG_EERIE_LEAP_LOG_METADATA_FILE_VERSION,
            .reserved = 0
        };

        return header;
    }
} __attribute__((packed, aligned(1))); // Ensure no padding

} // namespace eerie_leap::domain::logging_domain::models
