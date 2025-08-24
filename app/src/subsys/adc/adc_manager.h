#pragma once

#if defined(CONFIG_ADC) || defined(CONFIG_ADC_EMUL)

#include <functional>
#include <memory>
#include <vector>
#include <zephyr/drivers/adc.h>

#include "utilities/memory/heap_allocator.h"
#include "subsys/device_tree/adc_dt_info.h"
#include "models/adc_configuration.h"

#include "i_adc_manager.h"
#include "adc.h"

namespace eerie_leap::subsys::adc {

using namespace eerie_leap::utilities::memory;
using namespace eerie_leap::subsys::device_tree;
using namespace eerie_leap::subsys::adc::models;

class AdcManager : public IAdcManager {
private:
    std::vector<AdcDTInfo> adc_infos_;

protected:
    std::vector<std::shared_ptr<IAdc>> adcs_;
    std::shared_ptr<AdcConfiguration> adc_configuration_;

    bool IsChannelValid(int channel) {
        return adc_configuration_ != nullptr
            && channel >= 0
            && channel < adc_configuration_->channel_configurations->size()
            && channel < GetChannelCount();
    }

public:
    AdcManager(std::vector<AdcDTInfo> adc_infos) {
        adc_infos_ = adc_infos;

        for(auto& adc_info : adc_infos_) {
            adcs_.push_back(make_shared_ext<Adc>(adc_info));
        }
    }

    int Initialize() override {
        for(auto& adc : adcs_)
            adc->Initialize();

        return 0;
    }

    void UpdateConfiguration(std::shared_ptr<AdcConfiguration>& adc_configuration) override {
        adc_configuration_ = adc_configuration;

        for(auto& adc : adcs_)
            adc->UpdateConfiguration(adc_configuration->samples);
    }

    std::shared_ptr<AdcChannelConfiguration> GetChannelConfiguration(int channel) override {
        if(!IsChannelValid(channel)) {
            throw std::invalid_argument("ADC channel out of range.");
        }

        return adc_configuration_->channel_configurations->at(channel);
    }

    std::pair<IAdc*, int> GetAdcForChannel(int channel) {
        if(!IsChannelValid(channel))
            throw std::invalid_argument("ADC channel out of range.");

        int channel_index = channel;
        for(auto& adc : adcs_) {
            if(channel_index < adc->GetChannelCount())
                return std::make_pair(adc.get(), channel_index);
            channel_index -= adc->GetChannelCount();
        }

        throw std::invalid_argument("ADC channel out of range.");
    }

    std::function<float ()> GetChannelReader(int channel) override {
        IAdc* adc;
        int channel_index;
        std::tie(adc, channel_index) = GetAdcForChannel(channel);

        return [adc, channel_index]() { return adc->ReadChannel(channel_index); };
    }

    int GetAdcCount() override {
        return adcs_.size();
    }

    int GetChannelCount() override {
        int count = 0;
        for(auto& adc_info : adc_infos_)
            count += adc_info.channel_configs.size();

        return count;
    }

    void UpdateSamplesCount(int samples) override {
        for(auto& adc : adcs_)
            adc->UpdateConfiguration(samples);
    }

    void ResetSamplesCount() override {
        for(auto& adc : adcs_)
            adc->UpdateConfiguration(adc_configuration_->samples);
    }
};

}  // namespace eerie_leap::subsys::adc

#endif // defined(CONFIG_ADC) || defined(CONFIG_ADC_EMUL)
