#pragma once

#include <memory_resource>
#include <vector>

#include "interpolation_method.h"
#include "calibration_data.h"

namespace eerie_leap::utilities::voltage_interpolator {

class IVoltageInterpolator {
public:
    virtual ~IVoltageInterpolator() = default;

    virtual float Interpolate(float voltage, bool clamp_to_ends = false) const = 0;
    virtual const std::shared_ptr<std::pmr::vector<CalibrationData>> GetCalibrationTable() const = 0;
    virtual const InterpolationMethod GetInterpolationMethod() const = 0;
};

} // namespace eerie_leap::utilities::voltage_interpolator
