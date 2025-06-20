#pragma once

#include <cstdint>
#include <string>

namespace eerie_leap::domain::hardware::adc_domain::models {

struct AdcConfiguration {
    std::string name;
    uint16_t samples;
};

}  // namespace eerie_leap::domain::hardware::adc_domain::models
