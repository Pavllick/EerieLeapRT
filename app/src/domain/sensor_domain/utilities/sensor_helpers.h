#pragma once

#include <string>

namespace eerie_leap::domain::sensor_domain::utilities {

class SensorHelpers {
public:
    static bool IsIdValid(const std::string& id);
};

} // namespace eerie_leap::domain::sensor_domain::utilities
