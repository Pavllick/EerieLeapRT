#pragma once

#include <memory>
#include <vector>

#include "domain/sensor_domain/utilities/voltage_interpolator/interpolation_method.h"
#include "domain/sensor_domain/models/calibration_data.h"

namespace eerie_leap::domain::sensor_domain::utilities::voltage_interpolator {

using namespace eerie_leap::domain::sensor_domain::models;

class IVoltageInterpolator {
public:
    virtual float Interpolate(float voltage) const = 0;
    virtual const std::shared_ptr<std::vector<CalibrationData>> GetCalibrationTable() const = 0;
    virtual const InterpolationMethod GetInterpolationMethod() const = 0;
};

} // namespace eerie_leap::domain::sensor_domain::utilities::voltage_interpolator
