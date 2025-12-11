#pragma once

#include <memory_resource>
#include <string>

namespace eerie_leap::domain::sensor_domain::models {

struct SensorMetadata {
    using allocator_type = std::pmr::polymorphic_allocator<>;

    std::pmr::string name;
    std::pmr::string unit;
    std::pmr::string description;

    SensorMetadata(std::allocator_arg_t, allocator_type alloc) {}

    SensorMetadata(const SensorMetadata&) = delete;
	SensorMetadata& operator=(const SensorMetadata&) noexcept = default;
	SensorMetadata& operator=(SensorMetadata&&) noexcept = default;
	SensorMetadata(SensorMetadata&&) noexcept = default;
	~SensorMetadata() = default;

    SensorMetadata(SensorMetadata&& other, allocator_type alloc) noexcept
        : name(other.name, alloc),
        unit(other.unit, alloc),
        description(other.description, alloc) {}
};

} // namespace eerie_leap::domain::sensor_domain::models
