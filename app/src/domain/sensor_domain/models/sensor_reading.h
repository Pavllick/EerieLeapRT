#pragma once

#include <memory_resource>
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
    using allocator_type = std::pmr::polymorphic_allocator<>;

    const Guid id;
    const std::shared_ptr<Sensor> sensor;
    std::optional<float> value;
    std::optional<system_clock::time_point> timestamp;
    ReadingStatus status = ReadingStatus::UNINITIALIZED;
    std::optional<std::pmr::string> error_message;
    ReadingMetadata metadata;

    SensorReading(
        std::allocator_arg_t,
        allocator_type alloc,
        const Guid id,
        std::shared_ptr<Sensor> sensor)
            : id(id),
                sensor(std::move(sensor)),
                metadata(std::allocator_arg, alloc) {}

    SensorReading(const SensorReading&) = delete;
	SensorReading& operator=(const SensorReading&) noexcept = default;
	SensorReading& operator=(SensorReading&&) noexcept = default;
	SensorReading(SensorReading&&) noexcept = default;
	~SensorReading() = default;

    SensorReading(SensorReading&& other, allocator_type alloc) noexcept
        : id(other.id),
        sensor(other.sensor),
        value(other.value),
        timestamp(other.timestamp),
        status(other.status),
        error_message(other.error_message),
        metadata(std::move(other.metadata)) {}
};

}

// namespace eerie_leap::domain::sensor_domain::models
