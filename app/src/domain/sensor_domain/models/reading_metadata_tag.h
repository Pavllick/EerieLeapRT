#pragma once

#include <cstdint>

namespace eerie_leap::domain::sensor_domain::models {

enum class ReadingMetadataTag : uint8_t {
    VOLTAGE,
    RAW_VALUE
};

} // namespace eerie_leap::domain::sensor_domain::models
