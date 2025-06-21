#pragma once

#include <memory>
#include <vector>

#include "domain/hardware/adc_domain/i_adc.h"

namespace eerie_leap::domain::hardware::adc_domain {

using namespace eerie_leap::domain::hardware::adc_domain::models;

class IAdcManager {
public:
    virtual int Initialize() = 0;
    virtual void UpdateConfigurations(std::shared_ptr<std::vector<std::shared_ptr<AdcConfiguration>>>& adc_configurations) = 0;
    virtual std::shared_ptr<IAdc> GetAdcForChannel(int channel) = 0;
    virtual float ReadChannel(int channel) = 0;
    virtual int GetAdcCount() = 0;
    virtual int GetChannelCount() = 0;
};

}  // namespace eerie_leap::domain::hardware::adc_domain
