#pragma once

#include "adc_manager.h"

namespace eerie_leap::domain::hardware::adc_domain {

class AdcEmulator : public AdcManager {
public:
    int Initialize() override;
    float ReadChannel(int channel) override;
    int GetChannelCount() override;
};

}  // namespace eerie_leap::domain::hardware::adc_domain
