#pragma once

#include <vector>
#include <optional>

#include <zephyr/devicetree.h>
#include <zephyr/device.h>
#include "zephyr/drivers/gpio.h"

#if DT_HAS_ALIAS(gpioc)
#define GPIOC0_NODE DT_ALIAS(gpioc)
#define GPIO_SPEC(node_id) GPIO_DT_SPEC_GET(node_id, gpios)
#endif

namespace eerie_leap::subsys::device_tree {

class DtGpio {
private:
    static std::optional<std::vector<gpio_dt_spec>> gpio_specs_;

    DtGpio() = default;

public:
    static void Initialize();

    static std::optional<std::vector<gpio_dt_spec>>& Get() { return gpio_specs_; }
};

} // namespace eerie_leap::subsys::device_tree
