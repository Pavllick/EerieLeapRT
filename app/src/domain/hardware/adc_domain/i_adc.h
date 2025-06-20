#pragma once

#include "domain/hardware/adc_domain/models/adc_configuration.h"

namespace eerie_leap::domain::hardware::adc_domain {

using namespace eerie_leap::domain::hardware::adc_domain::models;

class IAdc {
public:
    virtual int Initialize() = 0;
    virtual void UpdateConfiguration(AdcConfiguration config) = 0;
    virtual float ReadChannel(int channel) = 0;
    virtual int GetChannelCount() = 0;
};

}  // namespace eerie_leap::domain::hardware::adc_domain
