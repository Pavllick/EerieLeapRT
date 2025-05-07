#pragma once

#include <cstdint>
#include <string>

namespace eerie_leap::domain::adc_domain::hardware {

struct AdcConfiguration {
    std::string name;
    uint16_t samples;
};

}  // namespace eerie_leap::domain::adc_domain::hardware
