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

float AdcEmulator::ReadChannel(int channel) {
    if(!adc_config_)
        throw std::runtime_error("ADC config is not set!");

    if(available_channels_.find(channel) == available_channels_.end())
        throw std::invalid_argument("ADC channel is not available.");

#ifdef CONFIG_ADC_EMUL
    uint32_t raw = sys_rand32_get();
    uint16_t input_mv = static_cast<uint16_t>((static_cast<uint64_t>(raw) * 3301) >> 32);
    int err = adc_emul_const_value_set(adc_device_, channel, input_mv);
    if(err < 0) {
        LOG_ERR("Could not set constant value (%d)", err);
        return 0;
    }
#endif

    return Adc::ReadChannel(channel);
}

}  // namespace eerie_leap::domain::adc_domain::hardware
