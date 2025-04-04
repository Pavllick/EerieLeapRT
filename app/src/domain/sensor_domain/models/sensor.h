#pragma once

#include <memory>
#include "sensor_id.h"
#include "sensor_metadata.h"
#include "sensor_configuration.h"

namespace eerie_leap::domain::sensor_domain::models {

struct Sensor {
    SensorId id;
    SensorMetadata metadata;
    SensorConfiguration configuration;
};

} // namespace eerie_leap::domain::sensor_domain::models