#pragma once

#include <memory>
#include <optional>
#include <string>

#include "calibration_data.h"
#include "sensor_type.h"
#include "utilities/math_parser/expression_evaluator.h"

namespace eerie_leap::domain::sensor_domain::models {

using namespace eerie_leap::utilities::math_parser;

struct SensorConfiguration {
    SensorType type;
    std::optional<int> channel;
    std::optional<CalibrationData> calibration;
    std::shared_ptr<ExpressionEvaluator> expression_evaluator = nullptr;
};

} // namespace eerie_leap::domain::sensor_domain::models