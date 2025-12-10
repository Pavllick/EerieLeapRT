#include "utilities/voltage_interpolator/linear_voltage_interpolator.hpp"
#include "utilities/voltage_interpolator/cubic_spline_voltage_interpolator.hpp"
#include "adc_calibrator.h"

namespace eerie_leap::subsys::adc::utilities {

using namespace eerie_leap::utilities::voltage_interpolator;

static const std::pmr::vector<CalibrationData> adc_inverse_base_range_data {
    {0.0, 0.0},
#ifdef CONFIG_ZTEST
    {CONFIG_EERIE_LEAP_ADC_VOLTAGE_MAX_MV / 1000.0f, CONFIG_EERIE_LEAP_ADC_VOLTAGE_MAX_MV / 1000.0f}
#else
    {CONFIG_EERIE_LEAP_SENSOR_VOLTAGE_MAX_MV / 1000.0f, CONFIG_EERIE_LEAP_ADC_VOLTAGE_MAX_MV / 1000.0f}
#endif
};
static const auto adc_inverse_base_range_data_ptr = std::make_shared<std::pmr::vector<CalibrationData>>(adc_inverse_base_range_data);
static const auto adc_inverse_base_range_voltage_interpolator = std::make_unique<LinearVoltageInterpolator>(adc_inverse_base_range_data_ptr);

AdcCalibrator::AdcCalibrator(
    InterpolationMethod interpolation_method,
    const std::shared_ptr<std::pmr::vector<CalibrationData>> calibration_data)
    : calibration_data_(std::move(calibration_data)) {

    std::pmr::vector<CalibrationData> adc_calibration_data_normalized;
    for(auto& calibration_data : *calibration_data_) {
        adc_calibration_data_normalized.push_back({
            .voltage = adc_inverse_base_range_voltage_interpolator->Interpolate(calibration_data.value),
            .value = calibration_data.voltage
        });
    }

    auto adc_calibration_data_normalized_ptr = std::make_shared<std::pmr::vector<CalibrationData>>(adc_calibration_data_normalized);

    switch (interpolation_method) {
    case InterpolationMethod::LINEAR:
        calibrated_voltage_interpolator_ = std::make_unique<LinearVoltageInterpolator>(adc_calibration_data_normalized_ptr);
        break;

    case InterpolationMethod::CUBIC_SPLINE:
        calibrated_voltage_interpolator_ = std::make_unique<CubicSplineVoltageInterpolator>(adc_calibration_data_normalized_ptr);
        break;

    default:
        throw std::runtime_error("Interpolation method is not supported.");
    }
}

static const std::pmr::vector<CalibrationData> adc_base_range_data {
    {0.0, 0.0},
    {CONFIG_EERIE_LEAP_ADC_VOLTAGE_MAX_MV / 1000.0f, CONFIG_EERIE_LEAP_SENSOR_VOLTAGE_MAX_MV / 1000.0f}
};
static const auto adc_base_range_data_ptr = std::make_shared<std::pmr::vector<CalibrationData>>(adc_base_range_data);
static const auto adc_base_range_voltage_interpolator = std::make_unique<LinearVoltageInterpolator>(adc_base_range_data_ptr);

float AdcCalibrator::InterpolateToInputRange(float value) {
    return adc_base_range_voltage_interpolator->Interpolate(value);
}

float AdcCalibrator::InterpolateToCalibratedRange(float value) {
    return calibrated_voltage_interpolator_->Interpolate(value);
}

InterpolationMethod AdcCalibrator::GetInterpolationMethod() const {
    return calibrated_voltage_interpolator_->GetInterpolationMethod();
}

std::shared_ptr<std::pmr::vector<CalibrationData>> AdcCalibrator::GetCalibrationTable() const {
    return calibration_data_;
}

} // namespace eerie_leap::subsys::adc::utilities
