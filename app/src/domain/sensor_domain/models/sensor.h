#pragma once

#include <string>
#include "sensor_metadata.h"
#include "sensor_configuration.h"

namespace eerie_leap::domain::sensor_domain::models {

struct Sensor {
    std::string id;
    SensorMetadata metadata;
    SensorConfiguration configuration;
};

} // namespace eerie_leap::domain::sensor_domain::models