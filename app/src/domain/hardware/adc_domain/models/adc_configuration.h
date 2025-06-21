#pragma once

#include <cstdint>
#include <memory>
#include <vector>

#include "adc_channel_configuration.h"

namespace eerie_leap::domain::hardware::adc_domain::models {

using namespace eerie_leap::utilities::voltage_interpolator;

struct AdcConfiguration {
    uint16_t samples;

    std::shared_ptr<std::vector<std::shared_ptr<AdcChannelConfiguration>>> channel_configurations = nullptr;
};

}  // namespace eerie_leap::domain::hardware::adc_domain::models
