#pragma once

namespace eerie_leap::domain::sensor_domain::models {
    
struct CalibrationData {
    double min_voltage;
    double max_voltage;
    double min_value;
    double max_value;
};

} // namespace eerie_leap::domain::sensor_domain::models