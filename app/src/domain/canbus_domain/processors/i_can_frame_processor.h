#pragma once

#include <memory>

#include "subsys/canbus/can_frame.h"
#include "domain/canbus_domain/models/can_message_configuration.h"

namespace eerie_leap::domain::canbus_domain::processors {

using namespace eerie_leap::subsys::canbus;
using namespace eerie_leap::domain::canbus_domain::models;

class ICanFrameProcessor {
public:
    virtual ~ICanFrameProcessor() = default;

    virtual CanFrame Process(
        const CanMessageConfiguration& message_configuration,
        const CanFrame& can_frame) = 0;
};

} // namespace eerie_leap::domain::canbus_domain::processors
