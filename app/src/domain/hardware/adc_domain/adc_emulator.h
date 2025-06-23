#pragma once

#include "adc_manager.h"
#include <functional>

namespace eerie_leap::domain::hardware::adc_domain {

#define ADC0_NODE DT_ALIAS(adc0)

class AdcEmulator : public AdcManager {
private:
    const device* adc_device_;
    float ReadChannel(int channel);

public:
    int Initialize() override;
    std::function<float ()> GetChannelReader(int channel) override;
};

}  // namespace eerie_leap::domain::hardware::adc_domain
