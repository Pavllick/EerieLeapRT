#pragma once

#include <cstdint>
#include <vector>

#include "subsys/canbus/canbus_type.h"

#include "can_message_configuration.h"

namespace eerie_leap::domain::canbus_domain::models {

using namespace eerie_leap::subsys::canbus;

struct CanChannelConfiguration {
    CanbusType type;
    uint8_t bus_channel;
    uint32_t bitrate;
    std::vector<CanMessageConfiguration> message_configurations;
};

} // namespace eerie_leap::domain::canbus_domain::models
