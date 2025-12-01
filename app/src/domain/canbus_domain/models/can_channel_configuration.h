#pragma once

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

#include "subsys/canbus/canbus_type.h"
#include "subsys/dbc/dbc.h"

#include "can_message_configuration.h"

namespace eerie_leap::domain::canbus_domain::models {

using namespace eerie_leap::subsys::canbus;
using namespace eerie_leap::subsys::dbc;

struct CanChannelConfiguration {
    CanbusType type;
    bool is_extended_id = false;
    uint8_t bus_channel;
    uint32_t bitrate;
    uint32_t data_bitrate;
    std::string dbc_file_path;
    std::vector<CanMessageConfiguration> message_configurations;

    std::shared_ptr<Dbc> dbc = std::make_shared<Dbc>();
};

} // namespace eerie_leap::domain::canbus_domain::models
