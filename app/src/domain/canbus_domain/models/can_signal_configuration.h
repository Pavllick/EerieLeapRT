#pragma once

#include <cstdint>
#include <string>

namespace eerie_leap::domain::canbus_domain::models {

struct CanSignalConfiguration {
    uint32_t start_bit;
    uint32_t size_bits;
    float factor = 1.0f;
    float offset = 0.0f;

    std::string name;
    std::string unit;
};

} // namespace eerie_leap::domain::canbus_domain::models
