#pragma once

#include <optional>
#include <stdio.h>

#include "adc_configuration.h"

namespace eerie_leap::domain::adc_domain::hardware {

class IAdc {
public:
    virtual int Initialize() = 0;
    virtual void UpdateConfiguration(AdcConfiguration config) = 0;
    virtual float ReadChannel(int channel) = 0;
};

}  // namespace eerie_leap::domain::adc_domain::hardware
