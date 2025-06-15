#pragma once

#include <cstdint>
#include <vector>
#include <zephyr/drivers/adc.h>

namespace eerie_leap::domain::hardware::adc_domain {

struct AdcDTInfo {
    const device* adc_device;
    std::vector<adc_channel_cfg> channel_configs;
    std::vector<uint32_t> references_mv;
    std::vector<uint8_t> resolutions;
};

} // namespace eerie_leap::domain::hardware::adc_domain
