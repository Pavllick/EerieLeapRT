#include <stdexcept>
#include <zephyr/logging/log.h>
#include <zephyr/random/random.h>

#include "adc.h"

namespace eerie_leap::domain::adc_domain::hardware {

LOG_MODULE_REGISTER(adc_logger);

int Adc::Initialize() { 
    LOG_INF("Adc initialization started.");

    LOG_INF("Adc initialized successfully.");

    return 0;
}

void Adc::UpdateConfiguration(AdcConfig config) {
    adc_config_ = config;
    LOG_INF("Adc configuration updated.");
}

double Adc::ReadChannel(int channel) {
    if(!adc_config_)
        throw std::runtime_error("ADC config is not set!");

    if(channel < 0 || channel > adc_config_->channel_count)
        throw std::invalid_argument("ADC channel out of range!");

    uint32_t raw = sys_rand32_get();
    double random_value = (raw / static_cast<double>(UINT32_MAX)) * 3.3;

    return random_value;
}

}  // namespace eerie_leap::domain::adc_domain::hardware