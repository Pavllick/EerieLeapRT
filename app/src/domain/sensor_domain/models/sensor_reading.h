#pragma once

#include <memory>
#include <optional>
#include <string>
#include <chrono>

#include "utilities/guid/guid.hpp"
#include "sensor.h"
#include "reading_status.h"
#include "reading_metadata.h"

namespace eerie_leap::domain::sensor_domain::models {

using namespace std::chrono;
using namespace eerie_leap::utilities::guid;

struct SensorReading {
    const Guid id;
    const std::shared_ptr<Sensor> sensor;
    std::optional<float> value;
    std::optional<system_clock::time_point> timestamp;
    ReadingStatus status = ReadingStatus::UNINITIALIZED;
    std::optional<std::string> error_message;
    ReadingMetadata metadata;

    SensorReading(const Guid id, const std::shared_ptr<Sensor> sensor) : id(id), sensor(std::move(sensor)) {}
};

}

// namespace eerie_leap::domain::sensor_domain::models
