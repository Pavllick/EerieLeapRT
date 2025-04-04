#pragma once

#include <optional>
#include <string>
#include "calibration_data.h"
#include "sensor_type.h"

namespace eerie_leap::domain::sensor_domain::models {

struct SensorConfiguration {
    SensorType type;
    std::optional<int> channel;
    std::optional<CalibrationData> calibration;
    std::optional<char*> conversion_expression;
};

} // namespace eerie_leap::domain::sensor_domain::models