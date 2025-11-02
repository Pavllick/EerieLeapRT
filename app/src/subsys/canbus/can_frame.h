#pragma once

#include <cstdint>
#include <vector>

namespace eerie_leap::subsys::canbus {

struct CanFrame {
    uint32_t id;
    bool is_transmit;
    bool is_can_fd;
    std::vector<uint8_t> data;
};

}  // namespace eerie_leap::subsys::canbus
