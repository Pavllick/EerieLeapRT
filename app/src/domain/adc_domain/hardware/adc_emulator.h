#pragma once

#include "adc.h"

namespace eerie_leap::domain::adc_domain::hardware {

class AdcEmulator : public Adc {
    int Initialize() override;
    double ReadChannel(int channel) override;
};

}  // namespace eerie_leap::domain::adc_domain::hardware