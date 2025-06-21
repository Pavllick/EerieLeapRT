#pragma once

#include <memory>
#include <cstdint>
#include <vector>
#include <optional>
#include <unordered_set>
#include <zephyr/drivers/adc.h>

#include "i_adc.h"
#include "domain/hardware/adc_domain/models/adc_configuration.h"
#include "adc_dt_info.h"

namespace eerie_leap::domain::hardware::adc_domain {

using namespace eerie_leap::domain::hardware::adc_domain::models;

class Adc : public IAdc {
private:
    std::vector<adc_channel_cfg> channel_configs_;
    std::vector<uint32_t> references_mv_;
    std::vector<uint8_t> resolutions_;

protected:
    std::shared_ptr<AdcConfiguration> adc_configuration_ = nullptr;
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
    void UpdateConfiguration(std::shared_ptr<AdcConfiguration> adc_configuration) override;
    std::shared_ptr<AdcConfiguration> GetConfiguration() override;
    float ReadChannel(int channel) override;
    inline int GetChannelCount() override;
};

}  // namespace eerie_leap::domain::hardware::adc_domain
