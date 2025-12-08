#pragma once

#include <memory>

#include "models/adc_channel_configuration.h"

namespace eerie_leap::subsys::adc {

using namespace eerie_leap::subsys::adc::models;

class IAdc {
public:
    virtual ~IAdc() = default;

    virtual int Initialize() = 0;
    virtual void UpdateConfiguration(uint16_t samples) = 0;
    virtual float ReadChannel(int channel) = 0;
    virtual int GetChannelCount() = 0;
};

}  // namespace eerie_leap::subsys::adc
