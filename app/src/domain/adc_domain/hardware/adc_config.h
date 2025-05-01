#pragma once

namespace eerie_leap::domain::adc_domain::hardware {

struct AdcConfig {
    int channel_count;
    uint8_t resolution;
    uint16_t samples;
    uint32_t sampling_interval_us;
};

}  // namespace eerie_leap::domain::adc_domain::hardware