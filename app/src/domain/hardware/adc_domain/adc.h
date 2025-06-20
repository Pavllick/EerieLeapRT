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
#include "adc_dt_info.h"

namespace eerie_leap::domain::hardware::adc_domain {

class Adc : public IAdc {
private:
    std::vector<adc_channel_cfg> channel_configs_;
    std::vector<uint32_t> references_mv_;
    std::vector<uint8_t> resolutions_;

protected:
    std::optional<AdcConfiguration> adc_config_;
    const device* adc_device_;
    std::vector<adc_sequence> sequences_;
    adc_sequence_options sequence_options_;
    uint32_t samples_buffer_ = 0;
    std::unordered_set<uint8_t> available_channels_;

public:
    Adc(AdcDTInfo adc_dt_info) {
        adc_device_ = adc_dt_info.adc_device;
        channel_configs_ = adc_dt_info.channel_configs;
        references_mv_ = adc_dt_info.references_mv;
        resolutions_ = adc_dt_info.resolutions;
    }

    int Initialize() override;
    void UpdateConfiguration(AdcConfiguration config) override;
    float ReadChannel(int channel) override;
    inline int GetChannelCount() override;
};

}  // namespace eerie_leap::domain::hardware::adc_domain
