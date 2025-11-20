#pragma once

#include <unordered_map>

#include "can_channel_configuration.h"

namespace eerie_leap::domain::canbus_domain::models {

struct CanbusConfiguration {
    std::unordered_map<uint8_t, CanChannelConfiguration> channel_configurations;
};

} // namespace eerie_leap::domain::canbus_domain::models
