#pragma once

#include <memory>

#include "subsys/adc/utilities/adc_calibrator.h"

namespace eerie_leap::subsys::adc::models {

using namespace eerie_leap::subsys::adc::utilities;

struct AdcChannelConfiguration {
    std::shared_ptr<AdcCalibrator> calibrator = nullptr;
};

}  // namespace eerie_leap::subsys::adc::models
