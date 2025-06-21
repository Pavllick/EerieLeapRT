#pragma once

#include <memory>

#include "domain/hardware/adc_domain/models/adc_channel_configuration.h"

namespace eerie_leap::domain::hardware::adc_domain {

using namespace eerie_leap::domain::hardware::adc_domain::models;

class IAdc {
public:
    virtual int Initialize() = 0;
    virtual void UpdateConfiguration(uint16_t samples) = 0;
    virtual float ReadChannel(int channel) = 0;
    virtual int GetChannelCount() = 0;
};

}  // namespace eerie_leap::domain::hardware::adc_domain
