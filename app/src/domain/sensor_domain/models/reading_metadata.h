#pragma once

#include <unordered_map>
#include <string>

namespace eerie_leap::domain::sensor_domain::models {

struct ReadingMetadata {
    std::unordered_map<std::string, std::string> tags;
};

} // namespace eerie_leap::domain::sensor_domain::models