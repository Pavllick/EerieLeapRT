#pragma once

#include <cstdint>
#include <chrono>

#include "subsys/time/time_helpers.hpp"
#include "domain/sensor_domain/models/sensor_reading.h"

namespace eerie_leap::domain::user_com_domain::types {

using namespace std::chrono;
using namespace eerie_leap::subsys::time;
using namespace eerie_leap::domain::sensor_domain::models;

struct SensorReadingDto {
    Guid id;
    uint32_t sensor_id_hash;
    milliseconds timestamp_ms;
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
