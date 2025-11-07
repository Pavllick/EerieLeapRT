#pragma once

#include <cstdint>

namespace eerie_leap::domain::system_domain::models {

struct ComUserConfiguration {
    uint64_t device_id;
    uint16_t server_id;
};

} // namespace eerie_leap::domain::system_domain::models
