#pragma once

#include <cstdint>

namespace eerie_leap::domain::system_domain::models {

struct ComUserConfiguration {
    uint64_t device_id;
    uint16_t server_id;
    uint32_t refresh_rate_ms;
};

} // namespace eerie_leap::domain::system_domain::models
