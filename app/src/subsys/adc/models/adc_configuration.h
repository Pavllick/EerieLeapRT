#pragma once

#include <cstdint>
#include <memory>
#include <vector>

#include "adc_channel_configuration.h"

namespace eerie_leap::subsys::adc::models {

using namespace eerie_leap::utilities::voltage_interpolator;

struct AdcConfiguration {
    uint16_t samples = 0;

    std::shared_ptr<std::vector<std::shared_ptr<AdcChannelConfiguration>>> channel_configurations = nullptr;
};

}  // namespace eerie_leap::subsys::adc::models
