#pragma once

#ifdef CONFIG_ADC_EMUL

#include "adc_manager.h"
#include <functional>

namespace eerie_leap::domain::hardware::adc_domain {

#define ADC0_NODE DT_ALIAS(adc0)

class AdcEmulatorManager : public AdcManager {
private:
    const device* adc_device_;
    float ReadChannel(int channel);

public:
    AdcEmulatorManager(std::vector<AdcDTInfo> adc_infos) : AdcManager(adc_infos) {}

    int Initialize() override;
    std::function<float ()> GetChannelReader(int channel) override;
};

}  // namespace eerie_leap::domain::hardware::adc_domain

#endif // CONFIG_ADC_EMUL
