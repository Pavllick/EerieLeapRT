#pragma once

#include <memory>
#include <vector>
#include <unordered_map>
#include <zephyr/drivers/adc.h>

#include "i_adc_manager.h"
#include "adc.h"
#include "domain/hardware/adc_domain/models/adc_configuration.h"
#include "adc_dt_info.h"

namespace eerie_leap::domain::hardware::adc_domain {

using namespace eerie_leap::domain::hardware::adc_domain::models;

#define ADC_NODE(idx) DT_ALIAS(adc ## idx)

#define ADC_CHANNEL_VREF(node_id) DT_PROP_OR(node_id, zephyr_vref_mv, 0)
#define ADC_CHANNEL_RESOLUTION(node_id) DT_PROP_OR(node_id, zephyr_resolution, 0)

class AdcManager : public IAdcManager {
private:
    std::vector<AdcDTInfo> adc_infos_;
    std::vector<std::shared_ptr<IAdc>> adcs_;

    std::vector<int> adc_channel_map_;

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
            adcs_.push_back(std::make_shared<Adc>(adc_info));
        }
    }

    int Initialize() override {
        for(int i = 0; i < adcs_.size(); i++) {
            adcs_[i]->Initialize();

            for(int j = 0; j < adcs_[i]->GetChannelCount(); j++)
                adc_channel_map_.push_back(i);
        }

        return 0;
    }

    void UpdateConfigurations(std::shared_ptr<std::vector<std::shared_ptr<AdcConfiguration>>>& adc_configurations) override {
        if(adc_configurations->size() != adcs_.size())
            throw std::runtime_error("ADC configuration size does not match ADC count");

        for(int i = 0; i < adcs_.size(); i++)
            adcs_[i]->UpdateConfiguration(adc_configurations->at(i));
    }

    std::shared_ptr<IAdc> GetAdcForChannel(int channel) override {
        if(channel < 0 || channel >= adc_channel_map_.size())
            throw std::invalid_argument("ADC channel out of range!");

        int adc_index = adc_channel_map_[channel];
        return adcs_[adc_index];
    }

    float ReadChannel(int channel) override {
        int channel_index = channel;
        for(auto& adc : adcs_) {
            if(channel_index < adc->GetChannelCount())
                return adc->ReadChannel(channel_index);
            channel_index -= adc->GetChannelCount();
        }

        throw std::invalid_argument("ADC channel out of range!");
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
