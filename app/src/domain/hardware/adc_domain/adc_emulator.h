#pragma once

#include "adc_manager.h"

namespace eerie_leap::domain::hardware::adc_domain {

#define ADC0_NODE DT_ALIAS(adc0)

class AdcEmulator : public AdcManager {
private:
    const device* adc_device_;

public:
    int Initialize() override;
    float ReadChannel(int channel) override;
    int GetChannelCount() override;
};

}  // namespace eerie_leap::domain::hardware::adc_domain
