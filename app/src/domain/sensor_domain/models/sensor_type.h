#pragma once

#include <cstdint>
#include <string>

namespace eerie_leap::domain::sensor_domain::models {

    enum class SensorType : std::uint8_t {
        PHYSICAL_ANALOG,
        VIRTUAL_ANALOG
    };

    constexpr const std::string SensorTypeNames[] = {
        "PHYSICAL_ANALOG",
        "VIRTUAL_ANALOG"
    };

    inline const std::string& GetSensorTypeName(SensorType type) {
        return SensorTypeNames[static_cast<std::uint8_t>(type)];
    }

} // namespace eerie_leap::domain::sensor_domain::models
