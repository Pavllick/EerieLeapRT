#include <zephyr/logging/log.h>
#include <zephyr/drivers/adc/adc_emul.h>
#include <zephyr/random/random.h>

#include "adc_emulator.h"

namespace eerie_leap::domain::hardware::adc_domain {

LOG_MODULE_REGISTER(adc_emulator_logger);

int AdcEmulator::Initialize() {
    int res = AdcManager::Initialize();

    LOG_INF("Adc Emulator initialized successfully.");

    return res;
}

float AdcEmulator::ReadChannel(int channel) {
    if(channel < 0 || channel > GetChannelCount())
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

    return AdcManager::ReadChannel(channel);
}

int AdcEmulator::GetChannelCount() {
    return 16;
}

}  // namespace eerie_leap::domain::hardware::adc_domain
