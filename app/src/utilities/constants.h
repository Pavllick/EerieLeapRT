#pragma once

#include <cstdint>

namespace eerie_leap::utilities::constants {

    namespace logging {

    static const uint32_t LOG_METADATA_FILE_TYPE = 0x4D4C4C45; // ELLM
    static const char* LOG_METADATA_FILE_EXTENSION = "elm";
    static const uint32_t LOG_DATA_FILE_TYPE = 0x444C4C45; // ELLD
    static const char* LOG_DATA_FILE_EXTENSION = "ell";

    static const uint32_t LOG_DATA_RECORD_START_MARK = 0x4C52534D; // LRSM

    } // namespace logging

} // namespace eerie_leap::utilities::constants
