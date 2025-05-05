#pragma once

#include <string>
#include <optional>

namespace eerie_leap::domain::sensor_domain::models {

struct SensorMetadata {
    std::string name;
    std::string unit;
    std::optional<std::string> description = std::nullopt;
};

} // namespace eerie_leap::domain::sensor_domain::models