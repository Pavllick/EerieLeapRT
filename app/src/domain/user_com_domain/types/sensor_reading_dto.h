#pragma once

#include "utilities/time/time_helpers.hpp"
#include "domain/sensor_domain/models/sensor_reading.h"

namespace eerie_leap::domain::user_com_domain::types {

using namespace eerie_leap::utilities::time;
using namespace eerie_leap::domain::sensor_domain::models;

struct SensorReadingDto {
    Guid id;
    size_t sensor_id_hash;
    std::chrono::milliseconds timestamp_ms;
    float value;
    ReadingStatus status;

    static SensorReadingDto FromSensorReading(const SensorReading& reading) {
        return SensorReadingDto{
            reading.id,
            reading.sensor->id_hash,
            TimeHelpers::ToMilliseconds(*reading.timestamp),
            reading.value.value_or(0.0f),
            reading.status
        };
    }
};

} // namespace eerie_leap::domain::user_com_domain::types
