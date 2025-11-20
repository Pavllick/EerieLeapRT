#pragma once

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

#include "utilities/memory/heap_allocator.h"
#include "subsys/canbus/canbus_type.h"
#include "subsys/dbc/dbc.h"

#include "can_message_configuration.h"

namespace eerie_leap::domain::canbus_domain::models {

using namespace eerie_leap::utilities::memory;
using namespace eerie_leap::subsys::canbus;
using namespace eerie_leap::subsys::dbc;

struct CanChannelConfiguration {
    CanbusType type;
    uint8_t bus_channel;
    uint32_t bitrate;
    std::string dbc_file_path;
    std::vector<CanMessageConfiguration> message_configurations;

    std::shared_ptr<Dbc> dbc = make_shared_ext<Dbc>();
};

} // namespace eerie_leap::domain::canbus_domain::models
