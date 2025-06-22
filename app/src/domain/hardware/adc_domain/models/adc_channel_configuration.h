#pragma once

#include <memory>

#include "domain/hardware/adc_domain/utilities/adc_calibrator.h"

namespace eerie_leap::domain::hardware::adc_domain::models {

using namespace eerie_leap::domain::hardware::adc_domain::utilities;

struct AdcChannelConfiguration {
    std::shared_ptr<AdcCalibrator> calibrator;
};

}  // namespace eerie_leap::domain::hardware::adc_domain::models
