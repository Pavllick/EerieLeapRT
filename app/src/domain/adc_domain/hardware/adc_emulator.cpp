#include <zephyr/logging/log.h>
#include <zephyr/drivers/adc/adc_emul.h>
#include <zephyr/random/random.h>

#include "adc_emulator.h"

namespace eerie_leap::domain::adc_domain::hardware {

LOG_MODULE_REGISTER(adc_emulator_logger);

int AdcEmulator::Initialize() {
    int res = Adc::Initialize();

    LOG_INF("Adc Emulator initialized successfully.");

    return res;
}

double AdcEmulator::ReadChannel(int channel) {
    if(!adc_config_)
        throw std::runtime_error("ADC config is not set!");

    if(channel < 0 || channel > adc_config_->channel_count)
        throw std::invalid_argument("ADC channel out of range!");

    uint32_t raw = sys_rand32_get();
    uint16_t input_mv = static_cast<uint16_t>((static_cast<uint64_t>(raw) * 3301) >> 32);
    int err = adc_emul_const_value_set(adc_device_, channel, input_mv);
    if(err < 0) {
        printf("Could not set constant value (%d)\n", err);
        return 0;
    }

    return Adc::ReadChannel(channel);
}

}  // namespace eerie_leap::domain::adc_domain::hardware