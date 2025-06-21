#pragma once

#include <cstdint>
#include <string>
#include <memory>

#include "utilities/voltage_interpolator/i_voltage_interpolator.h"

namespace eerie_leap::domain::hardware::adc_domain::models {

using namespace eerie_leap::utilities::voltage_interpolator;

struct AdcConfiguration {
    std::string name;
    uint16_t samples;

    // ADC calibration mechanism
    std::shared_ptr<IVoltageInterpolator> voltage_interpolator = nullptr;
};

}  // namespace eerie_leap::domain::hardware::adc_domain::models
