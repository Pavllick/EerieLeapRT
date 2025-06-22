#include "utilities/constants.h"
#include "utilities/memory/heap_allocator.h"
#include "utilities/voltage_interpolator/linear_voltage_interpolator.hpp"
#include "utilities/voltage_interpolator/cubic_spline_voltage_interpolator.hpp"
#include "adc_calibrator.h"

namespace eerie_leap::domain::hardware::adc_domain::utilities {

using namespace eerie_leap::utilities::constants;
using namespace eerie_leap::utilities::memory;
using namespace eerie_leap::utilities::voltage_interpolator;

static const std::vector<CalibrationData> adc_inverse_base_range_data {
    {0.0, 0.0},
    {adc::SENSOR_VOLTAGE_MAX, adc::ADC_VOLTAGE_MAX}
};
static const auto adc_inverse_base_range_data_ptr = make_shared_ext<std::vector<CalibrationData>>(adc_inverse_base_range_data);
static const auto adc_inverse_base_range_voltage_interpolator = make_shared_ext<LinearVoltageInterpolator>(adc_inverse_base_range_data_ptr);

AdcCalibrator::AdcCalibrator(
    InterpolationMethod interpolation_method,
    const std::shared_ptr<std::vector<CalibrationData>>& calibration_data)
    : calibration_data_(calibration_data) {

    std::vector<CalibrationData> adc_calibration_data_normalized;
    for(auto& calibration_data : *calibration_data_) {
        adc_calibration_data_normalized.push_back({
            .voltage = adc_inverse_base_range_voltage_interpolator->Interpolate(calibration_data.value),
            .value = calibration_data.voltage
        });
    }

    auto adc_calibration_data_normalized_ptr = make_shared_ext<std::vector<CalibrationData>>(adc_calibration_data_normalized);

    switch (interpolation_method) {
    case InterpolationMethod::LINEAR:
        calibrated_voltage_interpolator_ = make_shared_ext<LinearVoltageInterpolator>(adc_calibration_data_normalized_ptr);
        break;

    case InterpolationMethod::CUBIC_SPLINE:
        calibrated_voltage_interpolator_ = make_shared_ext<CubicSplineVoltageInterpolator>(adc_calibration_data_normalized_ptr);
        break;

    default:
        throw std::runtime_error("Interpolation method is not supported!");
    }
}

static const std::vector<CalibrationData> adc_base_range_data {
    {0.0, 0.0},
    {adc::ADC_VOLTAGE_MAX, adc::SENSOR_VOLTAGE_MAX}
};
static const auto adc_base_range_data_ptr = make_shared_ext<std::vector<CalibrationData>>(adc_base_range_data);
static const auto adc_base_range_voltage_interpolator = make_shared_ext<LinearVoltageInterpolator>(adc_base_range_data_ptr);

float AdcCalibrator::InterpolateToInputRange(float value) {
    return adc_base_range_voltage_interpolator->Interpolate(value);
}

float AdcCalibrator::InterpolateToCalibratedRange(float value) {
    return calibrated_voltage_interpolator_->Interpolate(value);
}

InterpolationMethod AdcCalibrator::GetInterpolationMethod() const {
    return calibrated_voltage_interpolator_->GetInterpolationMethod();
}

std::shared_ptr<std::vector<CalibrationData>> AdcCalibrator::GetCalibrationTable() const {
    return calibration_data_;
}

} // namespace eerie_leap::domain::hardware::adc_domain::utilities
