#pragma once

#include <unordered_map>

#include <zephyr/devicetree.h>
#include <zephyr/device.h>

#if defined(CONFIG_CAN) && DT_HAS_CHOSEN(zephyr_canbus)
#define CANBUS_NODE DT_CHOSEN(zephyr_canbus)
#endif

namespace eerie_leap::subsys::device_tree {

class DtCanbus {
private:
    static std::unordered_map<uint8_t, const device*> canbus_devs_;

    DtCanbus() = default;

public:
    static void Initialize();

    static const device* Get(uint8_t bus_channel);
};

} // namespace eerie_leap::subsys::device_tree
