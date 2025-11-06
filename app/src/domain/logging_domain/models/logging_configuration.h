#pragma once

#include <cstdint>
#include <unordered_map>

#include "sensor_logging_configuration.h"

namespace eerie_leap::domain::logging_domain::models {

struct LoggingConfiguration {
    uint32_t logging_interval_ms = 100;
    // TODO: Implement creation of new log files when the size exceeds this value
    uint32_t max_log_size_mb = 10;

    std::unordered_map<uint32_t, SensorLoggingConfiguration> sensor_configurations;
};

} // namespace eerie_leap::domain::logging_domain::models
