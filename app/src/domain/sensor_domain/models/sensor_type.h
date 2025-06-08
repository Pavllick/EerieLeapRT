#pragma once

#include <string>
#include <stdexcept>

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

    inline SensorType GetSensorType(const std::string& name) {
        for(size_t i = 0; i < size(SensorTypeNames); ++i)
            if(SensorTypeNames[i] == name)
                return static_cast<SensorType>(i);

        throw std::runtime_error("Invalid sensor type name");
    }

} // namespace eerie_leap::domain::sensor_domain::models
