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
#include "adc_configuration.h"

namespace eerie_leap::domain::hardware::adc_domain {

#define ADC_NODE DT_ALIAS(adc0)

#define CHANNEL_VREF(node_id) DT_PROP_OR(node_id, zephyr_vref_mv, 0)
#define CHANNEL_RESOLUTION(node_id) DT_PROP_OR(node_id, zephyr_resolution, 0)

class Adc : public IAdc {
private:
    std::vector<adc_channel_cfg> channel_configs_;
    std::vector<uint32_t> references_mv_;
    std::vector<uint8_t> resolutions_;

protected:
    std::optional<AdcConfiguration> adc_config_;
    const struct device* adc_device_;
    std::vector<adc_sequence> sequences_;
    adc_sequence_options sequence_options_;
    std::unique_ptr<uint16_t[]> samples_buffer_;
    std::unordered_set<uint8_t> available_channels_;
    uint16_t GetReding();

public:
    Adc() {
        channel_configs_ = {DT_FOREACH_CHILD_SEP(ADC_NODE, ADC_CHANNEL_CFG_DT, (,))};
        references_mv_ = {DT_FOREACH_CHILD_SEP(ADC_NODE, CHANNEL_VREF, (,))};
        resolutions_ = {DT_FOREACH_CHILD_SEP(ADC_NODE, CHANNEL_RESOLUTION, (,))};
    }

    int Initialize() override;
    void UpdateConfiguration(AdcConfiguration config) override;
    float ReadChannel(int channel) override;
    inline int GetChannelCount() override;
};

}  // namespace eerie_leap::domain::hardware::adc_domain
