#pragma once

#include <memory>
#include <zephyr/kernel.h>

#include "subsys/canbus/canbus.h"
#include "domain/canbus_domain/models/can_message_configuration.h"
#include "domain/canbus_domain/can_frame_builders/can_frame_dbc_builder.h"
#include "domain/canbus_domain/processors/i_can_frame_processor.h"

namespace eerie_leap::domain::canbus_domain::services {

using namespace eerie_leap::subsys::canbus;
using namespace eerie_leap::domain::canbus_domain::models;
using namespace eerie_leap::domain::canbus_domain::can_frame_builders;
using namespace eerie_leap::domain::canbus_domain::processors;

struct CanbusTask {
    k_timeout_t send_interval_ms;
    uint8_t bus_channel;
    CanMessageConfiguration message_configuration;
    std::shared_ptr<Canbus> canbus;
    std::shared_ptr<CanFrameDbcBuilder> can_frame_dbc_builder;
    std::shared_ptr<std::vector<std::shared_ptr<ICanFrameProcessor>>> can_frame_processors;
};

} // namespace eerie_leap::domain::canbus_domain::services
