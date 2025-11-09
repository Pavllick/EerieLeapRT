#pragma once

#include <cstdint>
#include <string>
#include <functional>

#include "sensor_metadata.h"
#include "sensor_configuration.h"

namespace eerie_leap::domain::sensor_domain::models {

struct Sensor {
    std::string id;
    uint32_t id_hash;
    SensorMetadata metadata;
    SensorConfiguration configuration;

    explicit Sensor(const std::string& id) : id(id) {
        std::hash<std::string> string_hasher;
        id_hash = static_cast<uint32_t>(string_hasher(id));
    }
};

} // namespace eerie_leap::domain::sensor_domain::models
