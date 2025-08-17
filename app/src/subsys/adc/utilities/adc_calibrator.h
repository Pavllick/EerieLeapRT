#pragma once

#include <memory>
#include <vector>

#include "utilities/voltage_interpolator/calibration_data.h"
#include "utilities/voltage_interpolator/i_voltage_interpolator.h"

namespace eerie_leap::subsys::adc::utilities {

using namespace eerie_leap::utilities::voltage_interpolator;

class AdcCalibrator {
private:
    std::shared_ptr<std::vector<CalibrationData>> calibration_data_;
    std::shared_ptr<IVoltageInterpolator> calibrated_voltage_interpolator_;

public:
    AdcCalibrator(InterpolationMethod interpolation_method, const std::shared_ptr<std::vector<CalibrationData>>& calibration_data);

    static float InterpolateToInputRange(float value);

    float InterpolateToCalibratedRange(float value);
    InterpolationMethod GetInterpolationMethod() const;
    std::shared_ptr<std::vector<CalibrationData>> GetCalibrationTable() const;
};

} // namespace eerie_leap::subsys::adc::utilities
