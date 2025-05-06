#pragma once

#include <memory>
#include <cstdint>
#include <optional>
#include <zephyr/drivers/adc.h>

#include "i_adc.h"
#include "adc_config.h"

namespace eerie_leap::domain::adc_domain::hardware {

#define ADC_NODE DT_ALIAS(adc0)
#define CHANNEL_VREF(node_id) DT_PROP_OR(node_id, zephyr_vref_mv, 0)

/* Data array of ADC channels for the specified ADC. */
const adc_channel_cfg channel_configs_[] = {
    DT_FOREACH_CHILD_SEP(ADC_NODE, ADC_CHANNEL_CFG_DT, (,))};

/* Get the number of channels defined on the DTS. */
// #define CHANNEL_COUNT ARRAY_SIZE(channel_configs_)

class Adc : public IAdc {
protected:
    std::optional<AdcConfig> adc_config_;
    const device* adc_device_;
    adc_sequence sequence_;
    adc_sequence_options sequence_options_;
    const size_t dt_adc_channel_count_ = ARRAY_SIZE(channel_configs_);

    std::unique_ptr<uint16_t[]> samples_buffer_;
    uint16_t GetReding();

public:
    Adc() = default;
    ~Adc() = default;

    int Initialize() override;
    void UpdateConfiguration(AdcConfig config) override;
    double ReadChannel(int channel) override;
};

}  // namespace eerie_leap::domain::adc_domain::hardware
