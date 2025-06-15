#pragma once

#include <memory>
#include <cstdint>
#include <iterator>
#include <vector>
#include <optional>
#include <array>
#include <unordered_set>
#include <zephyr/drivers/adc.h>

#include "i_adc.h"
#include "adc.h"
#include "adc_configuration.h"
#include "adc_dt_info.h"

namespace eerie_leap::domain::hardware::adc_domain {

#define ADC_NODE(idx) DT_ALIAS(adc ## idx)

#define ADC_CHANNEL_VREF(node_id) DT_PROP_OR(node_id, zephyr_vref_mv, 0)
#define ADC_CHANNEL_RESOLUTION(node_id) DT_PROP_OR(node_id, zephyr_resolution, 0)

class AdcManager : public IAdc {
private:
    std::vector<AdcDTInfo> adc_infos_;
    std::vector<std::unique_ptr<IAdc>> adcs_;

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
            adcs_.push_back(std::make_unique<Adc>(adc_info));
        }
    }

    int Initialize() override {
        for(auto& adc : adcs_)
            adc->Initialize();

        return 0;
    }

    void UpdateConfiguration(AdcConfiguration config) override {
        for(auto& adc : adcs_)
            adc->UpdateConfiguration(config);
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

    int GetChannelCount() override {
        int count = 0;
        for(auto& adc_info : adc_infos_)
            count += adc_info.channel_configs.size();

        return count;
    }
};

}  // namespace eerie_leap::domain::hardware::adc_domain
