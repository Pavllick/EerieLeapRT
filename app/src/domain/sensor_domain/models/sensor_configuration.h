#pragma once

#include <memory>
#include <vector>
#include <optional>
#include <string>

#include "calibration_data.h"
#include "sensor_type.h"
#include "utilities/math_parser/expression_evaluator.h"
#include "domain/sensor_domain/utilities/voltage_interpolator/interpolation_method.h"
#include "domain/sensor_domain/utilities/voltage_interpolator/i_voltage_interpolator.h"

namespace eerie_leap::domain::sensor_domain::models {

using namespace eerie_leap::utilities::math_parser;
using namespace eerie_leap::domain::sensor_domain::utilities::voltage_interpolator;

struct SensorConfiguration {
    SensorType type;
    std::optional<uint32_t> channel;
    uint32_t sampling_rate_ms = 100;
    InterpolationMethod interpolation_method = InterpolationMethod::NONE;

    std::shared_ptr<IVoltageInterpolator> voltage_interpolator = nullptr;
    std::shared_ptr<ExpressionEvaluator> expression_evaluator = nullptr;
};

} // namespace eerie_leap::domain::sensor_domain::models