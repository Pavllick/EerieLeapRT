#pragma once

#include <cstdint>

namespace eerie_leap::domain::adc_domain::hardware {

struct AdcConfig {
    int channel_count;
    uint8_t resolution;
    uint16_t samples;
};

}  // namespace eerie_leap::domain::adc_domain::hardware
