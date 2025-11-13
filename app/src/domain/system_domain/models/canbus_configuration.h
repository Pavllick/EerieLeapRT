#pragma once

#include <cstdint>

#include "subsys/canbus/canbus_type.h"

namespace eerie_leap::domain::system_domain::models {

using namespace eerie_leap::subsys::canbus;

struct CanbusConfiguration {
    CanbusType type;
    uint8_t bus_channel;
    uint32_t bitrate;
};

} // namespace eerie_leap::domain::system_domain::models
