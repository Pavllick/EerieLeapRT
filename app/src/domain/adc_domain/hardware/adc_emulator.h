#pragma once

#include "adc.h"

namespace eerie_leap::domain::adc_domain::hardware {

class AdcEmulator : public Adc {
public:
    int Initialize() override;
    float ReadChannel(int channel) override;
    int GetChannelCount() override;
};

}  // namespace eerie_leap::domain::adc_domain::hardware
