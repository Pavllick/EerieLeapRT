#pragma once

#include <memory>

#include "utilities/voltage_interpolator/i_voltage_interpolator.h"

namespace eerie_leap::domain::hardware::adc_domain::models {

using namespace eerie_leap::utilities::voltage_interpolator;

struct AdcChannelConfiguration {
    // ADC calibration mechanism
    std::shared_ptr<IVoltageInterpolator> voltage_interpolator;
};

}  // namespace eerie_leap::domain::hardware::adc_domain::models
