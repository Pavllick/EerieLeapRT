#pragma once

#include <zephyr/devicetree.h>
#include <zephyr/device.h>

#if defined(CONFIG_CAN) && DT_HAS_CHOSEN(zephyr_canbus)
#define CANBUS_NODE DT_CHOSEN(zephyr_canbus)
#endif

namespace eerie_leap::subsys::device_tree {

class DtCanbus {
private:
    static const device *canbus_dev_;

    DtCanbus() = default;

public:
    static void Initialize();

    static const device* Get() { return canbus_dev_; }
};

} // namespace eerie_leap::subsys::device_tree
