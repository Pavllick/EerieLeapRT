#pragma once

#include <zephyr/devicetree.h>
#include <zephyr/device.h>

namespace eerie_leap::subsys::canbus {

class Canbus {
private:
    const device *canbus_dev_;

public:
    explicit Canbus(const device *canbus_dev);
    virtual ~Canbus() = default;

    int Initialize();
};

}  // namespace eerie_leap::subsys::canbus
