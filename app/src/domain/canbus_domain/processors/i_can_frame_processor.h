#pragma once

#include <memory>
#include <vector>
#include <span>

#include "domain/canbus_domain/models/can_message_configuration.h"

namespace eerie_leap::domain::canbus_domain::processors {

using namespace eerie_leap::domain::canbus_domain::models;

class ICanFrameProcessor {
public:
    virtual ~ICanFrameProcessor() = default;

    virtual std::vector<uint8_t> Process(
        const CanMessageConfiguration& message_configuration,
        std::span<const uint8_t> can_frame_data) = 0;
};

} // namespace eerie_leap::domain::canbus_domain::processors
