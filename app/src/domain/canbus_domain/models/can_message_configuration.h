#pragma once

#include <cstdint>

namespace eerie_leap::domain::canbus_domain::models {

struct CanMessageConfiguration {
    uint32_t frame_id;
    uint32_t send_interval_ms;
};

} // namespace eerie_leap::domain::canbus_domain::models
