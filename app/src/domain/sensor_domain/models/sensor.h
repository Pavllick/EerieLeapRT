#pragma once

#include <memory_resource>
#include <string>

#include "utilities/string/string_helpers.h"

#include "sensor_metadata.h"
#include "sensor_configuration.h"

namespace eerie_leap::domain::sensor_domain::models {

using namespace eerie_leap::utilities::string;

struct Sensor {
    using allocator_type = std::pmr::polymorphic_allocator<>;

    std::pmr::string id;
    size_t id_hash = 0;
    SensorMetadata metadata;
    SensorConfiguration configuration;

    Sensor(
        std::allocator_arg_t, allocator_type alloc, std::string_view id)
            : id(id, alloc),
            id_hash(StringHelpers::GetHash(id)),
            metadata(std::allocator_arg, alloc),
            configuration(std::allocator_arg, alloc) {}

    Sensor(const Sensor&) = delete;
	Sensor& operator=(const Sensor&) noexcept = default;
	Sensor& operator=(Sensor&&) noexcept = default;
	Sensor(Sensor&&) noexcept = default;
	~Sensor() = default;

    Sensor(Sensor&& other, allocator_type alloc) noexcept
        : id(other.id, alloc),
        id_hash(other.id_hash),
        metadata(std::move(other.metadata), alloc),
        configuration(std::move(other.configuration), alloc) {}
};

} // namespace eerie_leap::domain::sensor_domain::models
