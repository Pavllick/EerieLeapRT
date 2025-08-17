#include <stdexcept>

#include <zephyr/logging/log.h>
#include <zephyr/drivers/gpio/gpio_emul.h>
#include <zephyr/random/random.h>

#include "gpio_emulator.h"

LOG_MODULE_REGISTER(gpio_emulator_logger);

namespace eerie_leap::subsys::gpio {

int GpioEmulator::Initialize() {
    int res = Gpio::Initialize();

    LOG_INF("Gpio Emulator initialized successfully.");

    return res;
}

bool GpioEmulator::ReadChannel(int channel) {
    if(channel < 0 || channel > GetChannelCount())
        throw std::invalid_argument("Gpio channel is not available.");

#ifdef CONFIG_GPIO_EMUL
    uint32_t raw = sys_rand32_get();
    bool input = (raw & 1) == 0;

    int err = gpio_emul_input_set(
        gpio_specs_[channel].port,
        gpio_specs_[channel].pin,
        input ? 0 : 1);

    if(err < 0) {
        LOG_ERR("Could not set emulator value (%d)", err);
        return false;
    }
#endif

    return Gpio::ReadChannel(channel);
}

int GpioEmulator::GetChannelCount() {
    return 16;
}

}  // namespace eerie_leap::subsys::gpio
