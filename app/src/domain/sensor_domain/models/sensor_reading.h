#pragma once

#include <optional>
#include <string>
#include <memory>
#include <time.h>
#include "sensor.h"
#include "reading_metadata.h"
#include "reading_status.h"

namespace eerie_leap::domain::sensor_domain::models {

    struct SensorReading {
        std::optional<std::string> id;
        std::unique_ptr<Sensor> sensor;
        std::optional<double> value;
        std::optional<time_t> timestamp;
        ReadingMetadata metadata;
        ReadingStatus status = ReadingStatus::UNINITIALIZED;
        std::optional<std::string> error_message;
    };

}