#include <zephyr/ztest.h>

#include "utilities/voltage_interpolator/interpolation_method.h"
#include "utilities/voltage_interpolator/cubic_spline_voltage_interpolator.hpp"

using namespace eerie_leap::utilities::voltage_interpolator;

ZTEST_SUITE(cubic_spline_voltage_interpolator, NULL, NULL, NULL, NULL, NULL);

ZTEST(cubic_spline_voltage_interpolator, test_GetInterpolationMethod) {
    std::vector<CalibrationData> calibration_data_1 {
        {0.0, 0.0},
        {10, 100.0}
    };
    auto calibration_data_1_ptr = std::make_shared<std::vector<CalibrationData>>(calibration_data_1);
    CubicSplineVoltageInterpolator voltage_interpolator(calibration_data_1_ptr);

    zassert_equal(voltage_interpolator.GetInterpolationMethod(), InterpolationMethod::CUBIC_SPLINE);
}

ZTEST(cubic_spline_voltage_interpolator, test_GetCalibrationTable) {
    std::vector<CalibrationData> calibration_data_1 {
        {0.0, 0.0},
        {10, 100.0}
    };
    auto calibration_data_1_ptr = std::make_shared<std::vector<CalibrationData>>(calibration_data_1);
    CubicSplineVoltageInterpolator voltage_interpolator(calibration_data_1_ptr);

    zassert_equal(voltage_interpolator.GetCalibrationTable(), calibration_data_1_ptr);
}

ZTEST(cubic_spline_voltage_interpolator, test_Interpolate) {
    std::vector<CalibrationData> calibration_data_1 {
        {0.0, 0.0},
        {10, 100.0}
    };
    auto calibration_data_1_ptr = std::make_shared<std::vector<CalibrationData>>(calibration_data_1);
    CubicSplineVoltageInterpolator voltage_interpolator_1(calibration_data_1_ptr);
    zassert_equal(voltage_interpolator_1.Interpolate(-1.23), 0);
    float value = voltage_interpolator_1.Interpolate(4.26);
    zassert_between_inclusive(value, 42.59, 42.61);
    zassert_equal(voltage_interpolator_1.Interpolate(12), 100.0);

    std::vector<CalibrationData> calibration_data_2 {
        {0.0, 1.0},
        {1.0, 29.0},
        {2.0, 111.0},
        {2.5, 162.0},
        {3.3, 200.0}
    };
    auto calibration_data_2_ptr = std::make_shared<std::vector<CalibrationData>>(calibration_data_2);
    CubicSplineVoltageInterpolator voltage_interpolator_2(calibration_data_2_ptr);
    zassert_equal(voltage_interpolator_2.Interpolate(-1.2), 1.0);
    value = voltage_interpolator_2.Interpolate(0.5);
    zassert_between_inclusive(value, 10.53, 10.54);
    value = voltage_interpolator_2.Interpolate(2.25);
    zassert_between_inclusive(value, 137.97, 137.99);
    zassert_equal(voltage_interpolator_2.Interpolate(2.5), 162.0);
    zassert_equal(voltage_interpolator_2.Interpolate(4), 200.0);
}
