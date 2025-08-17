#ifdef CONFIG_ADC_EMUL

#include <stdexcept>
#include <zephyr/logging/log.h>
#include <zephyr/drivers/adc/adc_emul.h>
#include <zephyr/random/random.h>

#include "adc_emulator.h"

namespace eerie_leap::subsys::adc {

LOG_MODULE_REGISTER(adc_emulator_logger);

int AdcEmulatorManager::Initialize() {
    int res = AdcManager::Initialize();

    adc_device_ = DEVICE_DT_GET(ADC0_NODE);

    LOG_INF("Adc Emulator initialized successfully.");

    return res;
}

float AdcEmulatorManager::ReadChannel(int channel) {
#ifdef CONFIG_ADC_EMUL
    uint32_t raw = sys_rand32_get();
    uint16_t input_mv = static_cast<uint16_t>((static_cast<uint64_t>(raw) * 3301) >> 32);
    int err = adc_emul_const_value_set(adc_device_, channel, input_mv);
    if(err < 0) {
        LOG_ERR("Could not set constant value (%d).", err);
        return 0;
    }
#endif

    int channel_index = channel;
    for(auto& adc : adcs_) {
        if(channel_index < adc->GetChannelCount())
            return adc->ReadChannel(channel_index);
        channel_index += adc->GetChannelCount();
    }

    throw std::invalid_argument("ADC channel out of range.");
}

std::function<float ()> AdcEmulatorManager::GetChannelReader(int channel) {
    if(adc_configuration_ == nullptr
        || channel < 0
        || channel >= adc_configuration_->channel_configurations->size()
        || channel > GetChannelCount())
            throw std::invalid_argument("ADC channel out of range.");

    return [this, channel]() { return ReadChannel(channel); };
}

}  // namespace eerie_leap::subsys::adc

#endif // CONFIG_ADC_EMUL
