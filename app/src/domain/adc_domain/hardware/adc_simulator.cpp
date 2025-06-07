#include <stdexcept>
#include <zephyr/logging/log.h>
#include <zephyr/random/random.h>

#include "adc_simulator.h"

namespace eerie_leap::domain::adc_domain::hardware {

LOG_MODULE_REGISTER(adc_simulator_logger);

int AdcSimulator::Initialize() {
    LOG_INF("Adc Simulator initialization started.");

    LOG_INF("Adc Simulator initialized successfully.");

    return 0;
}

void AdcSimulator::UpdateConfiguration(AdcConfiguration config) {
    adc_config_ = config;
    LOG_INF("Adc configuration updated.");
}

float AdcSimulator::ReadChannel(int channel) {
    if(!adc_config_)
        throw std::runtime_error("ADC config is not set!");

    if(channel < 0 || channel > 31)
        throw std::invalid_argument("ADC channel out of range!");

    uint32_t raw = sys_rand32_get();
    float random_value = (raw / static_cast<float>(UINT32_MAX)) * 3.3;

    return random_value;
}

}  // namespace eerie_leap::domain::adc_domain::hardware
