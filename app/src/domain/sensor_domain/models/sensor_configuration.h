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
    std::optional<std::string> conversion_expression_raw;
    std::optional<std::string> conversion_expression_sanitized;
};

} // namespace eerie_leap::domain::sensor_domain::models