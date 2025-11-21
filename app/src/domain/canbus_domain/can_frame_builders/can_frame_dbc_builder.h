#pragma once

#include <memory>

#include "domain/canbus_domain/services/canbus_service.h"
#include "domain/sensor_domain/utilities/sensor_readings_frame.hpp"

namespace eerie_leap::domain::canbus_domain::can_frame_builders {

using namespace eerie_leap::domain::canbus_domain::services;
using namespace eerie_leap::domain::sensor_domain::utilities;

class CanFrameDbcBuilder {
private:
    std::shared_ptr<CanbusService> canbus_service_;
    std::shared_ptr<SensorReadingsFrame> sensor_readings_frame_;

public:
    CanFrameDbcBuilder(
        std::shared_ptr<CanbusService> canbus_service,
        std::shared_ptr<SensorReadingsFrame> sensor_readings_frame);

    CanFrame Build(uint8_t bus_channel, uint32_t frame_id);
};

} // namespace eerie_leap::domain::canbus_domain::can_frame_builders
