#pragma once

namespace eerie_leap::domain::adc_domain::hardware {

struct AdcConfig {
    int channel_count;
    int resolution;
    double reference_voltage;
};

}  // namespace eerie_leap::domain::adc_domain::hardware