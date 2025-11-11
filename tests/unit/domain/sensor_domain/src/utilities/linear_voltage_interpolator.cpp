#include <zephyr/ztest.h>

#include "utilities/voltage_interpolator/interpolation_method.h"
#include "utilities/voltage_interpolator/linear_voltage_interpolator.hpp"

using namespace eerie_leap::utilities::voltage_interpolator;

ZTEST_SUITE(linear_voltage_interpolator, NULL, NULL, NULL, NULL, NULL);

ZTEST(linear_voltage_interpolator, test_GetInterpolationMethod) {
    std::vector<CalibrationData> calibration_data_1 {
        {0.0, 0.0},
        {10, 100.0}
    };
    auto calibration_data_1_ptr = std::make_shared<std::vector<CalibrationData>>(calibration_data_1);
    LinearVoltageInterpolator voltage_interpolator(calibration_data_1_ptr);

    zassert_equal(voltage_interpolator.GetInterpolationMethod(), InterpolationMethod::LINEAR);
}

ZTEST(linear_voltage_interpolator, test_GetCalibrationTable) {
    std::vector<CalibrationData> calibration_data_1 {
        {0.0, 0.0},
        {10, 100.0}
    };
    auto calibration_data_1_ptr = std::make_shared<std::vector<CalibrationData>>(calibration_data_1);
    LinearVoltageInterpolator voltage_interpolator(calibration_data_1_ptr);

    zassert_equal(voltage_interpolator.GetCalibrationTable(), calibration_data_1_ptr);
}

ZTEST(linear_voltage_interpolator, test_Interpolate) {
    std::vector<CalibrationData> calibration_data_1 {
        {0.0, 0.0},
        {10, 100.0}
    };
    auto calibration_data_1_ptr = std::make_shared<std::vector<CalibrationData>>(calibration_data_1);
    LinearVoltageInterpolator voltage_interpolator_1(calibration_data_1_ptr);
    zassert_equal(voltage_interpolator_1.Interpolate(-1.23, true), 0);
    zassert_between_inclusive(voltage_interpolator_1.Interpolate(4.26, true), 42.59, 42.61);
    zassert_equal(voltage_interpolator_1.Interpolate(12, true), 100.0);

    std::vector<CalibrationData> calibration_data_2 {
        {0.0, 1.0},
        {1.0, 29.0},
        {2.0, 111.0},
        {2.5, 162.0},
        {3.3, 200.0}
    };
    auto calibration_data_2_ptr = std::make_shared<std::vector<CalibrationData>>(calibration_data_2);
    LinearVoltageInterpolator voltage_interpolator_2(calibration_data_2_ptr);
    zassert_equal(voltage_interpolator_2.Interpolate(-1.2, true), 1.0);
    zassert_equal(voltage_interpolator_2.Interpolate(0.5, true), 15.0);
    zassert_equal(voltage_interpolator_2.Interpolate(2.25, true), 136.5);
    zassert_equal(voltage_interpolator_2.Interpolate(2.5, true), 162.0);
    zassert_equal(voltage_interpolator_2.Interpolate(4, true), 200.0);
}
