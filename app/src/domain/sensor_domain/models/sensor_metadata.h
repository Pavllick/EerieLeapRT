#pragma once

#include <string>

namespace eerie_leap::domain::sensor_domain::models {

struct SensorMetadata {
    int id;
    std::string name;
    std::string unit;
    std::string sampling_rate_ms;
    std::string description;
};

} // namespace eerie_leap::domain::sensor_domain::models