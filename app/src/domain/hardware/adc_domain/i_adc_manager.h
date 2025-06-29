#pragma once

#include <functional>
#include <memory>
#include <vector>

#include "domain/hardware/adc_domain/models/adc_configuration.h"

namespace eerie_leap::domain::hardware::adc_domain {

using namespace eerie_leap::domain::hardware::adc_domain::models;

class IAdcManager {
public:
    virtual int Initialize() = 0;
    virtual void UpdateConfiguration(std::shared_ptr<AdcConfiguration>& adc_configuration) = 0;
    virtual std::shared_ptr<AdcChannelConfiguration> GetChannelConfiguration(int channel) = 0;
    virtual int GetAdcCount() = 0;
    virtual int GetChannelCount() = 0;
    virtual std::function<float ()> GetChannelReader(int channel) = 0;
    virtual void UpdateSamplesCount(int samples) = 0;
    virtual void ResetSamplesCount() = 0;
};

}  // namespace eerie_leap::domain::hardware::adc_domain
