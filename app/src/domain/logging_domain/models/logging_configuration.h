#pragma once

#include <memory_resource>
#include <cstdint>
#include <unordered_map>

#include "sensor_logging_configuration.h"

namespace eerie_leap::domain::logging_domain::models {

struct LoggingConfiguration {
    using allocator_type = std::pmr::polymorphic_allocator<std::byte>;

    uint32_t logging_interval_ms = 100;
    // TODO: Implement creation of new log files when the size exceeds this value
    uint32_t max_log_size_mb = 10;

    std::pmr::unordered_map<uint32_t, SensorLoggingConfiguration> sensor_configurations;

    LoggingConfiguration(std::allocator_arg_t, const allocator_type& alloc)
        : sensor_configurations(alloc) {}

    LoggingConfiguration(const LoggingConfiguration&) = delete;
    LoggingConfiguration& operator=(const LoggingConfiguration&) = delete;

    LoggingConfiguration(LoggingConfiguration&& other) noexcept
        : logging_interval_ms(other.logging_interval_ms),
          max_log_size_mb(other.max_log_size_mb),
          sensor_configurations(std::move(other.sensor_configurations)) {}

    LoggingConfiguration(LoggingConfiguration&& other, const allocator_type& alloc)
        : logging_interval_ms(other.logging_interval_ms),
          max_log_size_mb(other.max_log_size_mb),
          sensor_configurations(std::move(other.sensor_configurations), alloc.resource()) {}
};

} // namespace eerie_leap::domain::logging_domain::models
