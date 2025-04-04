#pragma once

namespace eerie_leap::domain::adc_domain::hardware {

struct AdcConfig {
    int channel_count;
    int resolution;
    int reference_voltage;
};

}  // namespace eerie_leap::domain::adc_domain::hardware