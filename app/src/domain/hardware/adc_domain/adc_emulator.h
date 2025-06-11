#pragma once

#include "adc.h"

namespace eerie_leap::domain::hardware::adc_domain {

class AdcEmulator : public Adc {
public:
    int Initialize() override;
    float ReadChannel(int channel) override;
    int GetChannelCount() override;
};

}  // namespace eerie_leap::domain::hardware::adc_domain
