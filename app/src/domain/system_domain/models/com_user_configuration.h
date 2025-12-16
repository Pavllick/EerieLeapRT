#pragma once

#include <cstdint>

namespace eerie_leap::domain::system_domain::models {

struct ComUserConfiguration {
    uint64_t device_id = 0;
    uint16_t server_id = 0;
};

} // namespace eerie_leap::domain::system_domain::models
