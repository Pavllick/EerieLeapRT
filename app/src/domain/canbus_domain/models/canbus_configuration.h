#pragma once

#include <vector>

#include "can_channel_configuration.h"

namespace eerie_leap::domain::canbus_domain::models {

struct CanbusConfiguration {
    std::vector<CanChannelConfiguration> channel_configurations;
};

} // namespace eerie_leap::domain::canbus_domain::models
