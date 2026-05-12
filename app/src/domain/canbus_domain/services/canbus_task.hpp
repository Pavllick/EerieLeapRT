#pragma once

#include <memory>
#include <zephyr/kernel.h>

#include "subsys/canbus/canbus_proxy.hpp"
#include "domain/canbus_domain/models/can_message_configuration.h"
#include "domain/canbus_domain/can_frame_builders/can_frame_dbc_builder.h"
#include "domain/canbus_domain/processors/i_can_frame_processor.h"

namespace eerie_leap::domain::canbus_domain::services {

using eerie_leap::subsys::canbus::CanbusProxy;
using eerie_leap::domain::canbus_domain::models::CanMessageConfiguration;
using eerie_leap::domain::canbus_domain::can_frame_builders::CanFrameDbcBuilder;
using eerie_leap::domain::canbus_domain::processors::ICanFrameProcessor;

struct CanbusTask {
    k_timeout_t send_interval_ms;
    uint8_t bus_channel;
    std::shared_ptr<CanMessageConfiguration> message_configuration;
    std::shared_ptr<CanbusProxy> canbus;
    std::shared_ptr<CanFrameDbcBuilder> can_frame_dbc_builder;
    std::shared_ptr<std::vector<std::shared_ptr<ICanFrameProcessor>>> can_frame_processors;
};

} // namespace eerie_leap::domain::canbus_domain::services
