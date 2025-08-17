#include <stdexcept>
#include <zephyr/logging/log.h>
#include <zephyr/random/random.h>

#include "gpio_simulator.h"

namespace eerie_leap::subsys::gpio {

LOG_MODULE_REGISTER(gpio_simulator_logger);

int GpioSimulator::Initialize() {
    LOG_INF("Gpio Simulator initialization started.");

    LOG_INF("Gpio Simulator initialized successfully.");

    return 0;
}

bool GpioSimulator::ReadChannel(int channel) {
    if(channel < 0 || channel > 31)
        throw std::invalid_argument("Gpio channel out of range.");

    uint32_t raw = sys_rand32_get();
    float random_value = (raw / static_cast<float>(UINT32_MAX)) * 3.3;

    return random_value > 1.65;
}

int GpioSimulator::GetChannelCount() {
    return 32;
}

}  // namespace eerie_leap::subsys::gpio
