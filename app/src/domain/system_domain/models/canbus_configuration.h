#pragma once

#include <cstdint>

namespace eerie_leap::domain::system_domain::models {

struct CanbusConfiguration {
    uint8_t bus_channel;
    uint32_t bitrate;
    uint32_t sampling_point_percent = 875;
};

} // namespace eerie_leap::domain::system_domain::models
