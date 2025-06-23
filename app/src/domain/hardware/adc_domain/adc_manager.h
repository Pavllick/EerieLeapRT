#pragma once

#include <functional>
#include <memory>
#include <vector>
#include <zephyr/drivers/adc.h>

#include "utilities/memory/heap_allocator.h"
#include "domain/hardware/adc_domain/models/adc_configuration.h"
#include "i_adc_manager.h"
#include "adc.h"
#include "adc_dt_info.h"

namespace eerie_leap::domain::hardware::adc_domain {

using namespace eerie_leap::utilities::memory;
using namespace eerie_leap::domain::hardware::adc_domain::models;

#define ADC_NODE(idx) DT_ALIAS(adc ## idx)

#define ADC_CHANNEL_VREF(node_id) DT_PROP_OR(node_id, zephyr_vref_mv, 0)
#define ADC_CHANNEL_RESOLUTION(node_id) DT_PROP_OR(node_id, zephyr_resolution, 0)

class AdcManager : public IAdcManager {
private:
    std::vector<AdcDTInfo> adc_infos_;

    std::shared_ptr<AdcConfiguration> adc_configuration_;

protected:
    std::vector<std::shared_ptr<IAdc>> adcs_;

public:
    AdcManager() {
    #if DT_HAS_ALIAS(adc0)
        AdcDTInfo adc_info0 = {
            .adc_device = DEVICE_DT_GET(ADC_NODE(0)),
            .channel_configs = {DT_FOREACH_CHILD_SEP(ADC_NODE(0), ADC_CHANNEL_CFG_DT, (,))},
            .references_mv = {DT_FOREACH_CHILD_SEP(ADC_NODE(0), ADC_CHANNEL_VREF, (,))},
            .resolutions = {DT_FOREACH_CHILD_SEP(ADC_NODE(0), ADC_CHANNEL_RESOLUTION, (,))}
        };
        adc_infos_.push_back(adc_info0);
    #endif

    #if DT_HAS_ALIAS(adc1)
        AdcDTInfo adc_info1 = {
            .adc_device = DEVICE_DT_GET(ADC_NODE(1)),
            .channel_configs = {DT_FOREACH_CHILD_SEP(ADC_NODE(1), ADC_CHANNEL_CFG_DT, (,))},
            .references_mv = {DT_FOREACH_CHILD_SEP(ADC_NODE(1), ADC_CHANNEL_VREF, (,))},
            .resolutions = {DT_FOREACH_CHILD_SEP(ADC_NODE(1), ADC_CHANNEL_RESOLUTION, (,))}
        };
        adc_infos_.push_back(adc_info1);
    #endif

    #if DT_HAS_ALIAS(adc2)
        AdcDTInfo adc_info2 = {
            .adc_device = DEVICE_DT_GET(ADC_NODE(2)),
            .channel_configs = {DT_FOREACH_CHILD_SEP(ADC_NODE(2), ADC_CHANNEL_CFG_DT, (,))},
            .references_mv = {DT_FOREACH_CHILD_SEP(ADC_NODE(2), ADC_CHANNEL_VREF, (,))},
            .resolutions = {DT_FOREACH_CHILD_SEP(ADC_NODE(2), ADC_CHANNEL_RESOLUTION, (,))}
        };
        adc_infos_.push_back(adc_info2);
    #endif

    #if DT_HAS_ALIAS(adc3)
        AdcDTInfo adc_info3 = {
            .adc_device = DEVICE_DT_GET(ADC_NODE(3)),
            .channel_configs = {DT_FOREACH_CHILD_SEP(ADC_NODE(3), ADC_CHANNEL_CFG_DT, (,))},
            .references_mv = {DT_FOREACH_CHILD_SEP(ADC_NODE(3), ADC_CHANNEL_VREF, (,))},
            .resolutions = {DT_FOREACH_CHILD_SEP(ADC_NODE(3), ADC_CHANNEL_RESOLUTION, (,))}
        };
        adc_infos_.push_back(adc_info3);
    #endif

        for(auto& adc_info : adc_infos_) {
            adcs_.push_back(make_shared_ext<Adc>(adc_info));
        }
    }

    int Initialize() override {
        for(int i = 0; i < adcs_.size(); i++)
            adcs_[i]->Initialize();

        return 0;
    }

    void UpdateConfiguration(std::shared_ptr<AdcConfiguration>& adc_configuration) override {
        adc_configuration_ = adc_configuration;

        for(auto& adc : adcs_)
            adc->UpdateConfiguration(adc_configuration->samples);
    }

    std::shared_ptr<AdcChannelConfiguration> GetChannelConfiguration(int channel) override {
        if(adc_configuration_ == nullptr
            || adc_configuration_->channel_configurations == nullptr
            || channel < 0
            || channel >= adc_configuration_->channel_configurations->size()) {

            throw std::invalid_argument("ADC channel out of range.");
        }

        return adc_configuration_->channel_configurations->at(channel);
    }

    std::pair<IAdc*, int> GetAdcForChannel(int channel) {
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
};

}  // namespace eerie_leap::domain::hardware::adc_domain
