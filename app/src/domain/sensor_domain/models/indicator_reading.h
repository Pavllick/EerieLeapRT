#pragma once

#include <memory>
#include <optional>
#include <string>
#include <chrono>

#include "sensor.h"
#include "reading_metadata.h"
#include "reading_status.h"
#include "utilities/guid/guid.hpp"

namespace eerie_leap::domain::sensor_domain::models {

using namespace std::chrono;
using namespace eerie_leap::utilities::guid;

struct IndicatorReading {
    const Guid id;
    const std::shared_ptr<Sensor> sensor;
    std::optional<bool> value;
    std::optional<system_clock::time_point> timestamp;
    ReadingMetadata metadata;
    ReadingStatus status = ReadingStatus::UNINITIALIZED;
    std::optional<std::string> error_message;

    IndicatorReading(const Guid id, const std::shared_ptr<Sensor> sensor) : id(id), sensor(std::move(sensor)) {}
};

}
