#pragma once

#include <memory>
#include <vector>
#include <algorithm>

#include "i_voltage_interpolator.h"
#include "domain/sensor_domain/models/calibration_data.h"

namespace eerie_leap::domain::sensor_domain::utilities::voltage_interpolator {

using namespace eerie_leap::domain::sensor_domain::models;

class LinearVoltageInterpolator : public IVoltageInterpolator {
private:
    static const InterpolationMethod INTERPOLATION_METHOD = InterpolationMethod::LINEAR;
    std::shared_ptr<std::vector<CalibrationData>> calibration_table_;

public:
    LinearVoltageInterpolator(const std::shared_ptr<std::vector<CalibrationData>>& calibration_table)
        : calibration_table_(std::move(calibration_table)) {

            if(!calibration_table_ || calibration_table_->size() < 2)
                throw std::invalid_argument("Calibration data is missing or invalid!");
        }

    double Interpolate(double voltage) const override {
        const auto& table = *calibration_table_;
    
        // Edge cases: clamp to ends
        if(voltage <= table.front().voltage)
            return table.front().value;
        if(voltage >= table.back().voltage)
            return table.back().value;
    
        // Binary search for the correct interval
        auto upper = std::lower_bound(
            table.begin(), table.end(), voltage,
            [](const CalibrationData& data, double v) {
                return data.voltage < v;
            });
    
        auto lower = upper - 1;
    
        double x0 = lower->voltage;
        double x1 = upper->voltage;
        double y0 = lower->value;
        double y1 = upper->value;
    
        double ratio = (voltage - x0) / (x1 - x0);
        return y0 + ratio * (y1 - y0);
    }

    const std::shared_ptr<std::vector<CalibrationData>> GetCalibrationTable() const override {
        return calibration_table_;
    }

    const InterpolationMethod GetInterpolationMethod() const override {
        return INTERPOLATION_METHOD;
    }
};

} // namespace eerie_leap::domain::sensor_domain::utilities::voltage_interpolator