#pragma once

#include <string>

#include "utilities/string/string_helpers.h"

#include "sensor_metadata.h"
#include "sensor_configuration.h"

namespace eerie_leap::domain::sensor_domain::models {

using namespace eerie_leap::utilities::string;

struct Sensor {
    std::string id;
    size_t id_hash;
    SensorMetadata metadata;
    SensorConfiguration configuration;

    explicit Sensor(const std::string& id) : id(id), id_hash(StringHelpers::GetHash(id)) {}
};

} // namespace eerie_leap::domain::sensor_domain::models
