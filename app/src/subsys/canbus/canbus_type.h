#pragma once

#include <cstdint>

namespace eerie_leap::subsys::canbus {

enum class CanbusType : uint8_t {
    CLASSICAL_CAN,
    CANFD
};

}  // namespace eerie_leap::subsys::canbus
