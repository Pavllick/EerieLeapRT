#pragma once

#include <memory>
#include <cstdint>
#include <optional>
#include <array>
#include <unordered_set>
#include <zephyr/drivers/adc.h>

#include "i_adc.h"
#include "adc_config.h"

namespace eerie_leap::domain::adc_domain::hardware {

#define ADC_NODE DT_ALIAS(adc0)
#define CHANNEL_VREF(node_id) DT_PROP_OR(node_id, zephyr_vref_mv, 0)

// Get resolution from first channel
#define CHANNEL_RESOLUTION(node_id) DT_PROP_OR(node_id, zephyr_resolution, 0)
#define ADC_RESOLUTION (0 DT_FOREACH_CHILD(ADC_NODE, CHANNEL_RESOLUTION))

/* Data array of ADC channels for the specified ADC. */
static const adc_channel_cfg channel_configs_[] = {DT_FOREACH_CHILD_SEP(ADC_NODE, ADC_CHANNEL_CFG_DT, (,))};

static const size_t dt_adc_channel_count_ = ARRAY_SIZE(channel_configs_);

/* Data array of ADC channel voltage references. */
static uint32_t references_mv_[] = {DT_FOREACH_CHILD_SEP(ADC_NODE, CHANNEL_VREF, (,))};

/* Data array of ADC channel resolutions. */
static const uint8_t resolutions_[] = {DT_FOREACH_CHILD_SEP(ADC_NODE, CHANNEL_RESOLUTION, (,))};

class Adc : public IAdc {
protected:
    std::optional<AdcConfig> adc_config_;
    const struct device* adc_device_;
    std::array<adc_sequence, dt_adc_channel_count_> sequences_;
    adc_sequence_options sequence_options_;
    std::unique_ptr<uint16_t[]> samples_buffer_;
    std::unordered_set<uint8_t> available_channels_;
    uint16_t GetReding();

public:
    Adc() = default;
    ~Adc() = default;

    int Initialize() override;
    void UpdateConfiguration(AdcConfig config) override;
    double ReadChannel(int channel) override;
};

}  // namespace eerie_leap::domain::adc_domain::hardware
