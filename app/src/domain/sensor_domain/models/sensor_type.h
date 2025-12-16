#pragma once

#include <cstdint>
#include <array>
#include <stdexcept>
#include <utility>
#include <string_view>

namespace eerie_leap::domain::sensor_domain::models {

using namespace std::string_view_literals;

enum class SensorType : std::uint32_t {
    NONE,
    PHYSICAL_ANALOG,
    VIRTUAL_ANALOG,
    PHYSICAL_INDICATOR,
    VIRTUAL_INDICATOR,
    CANBUS_RAW,
    CANBUS_ANALOG,
    CANBUS_INDICATOR,
    USER_ANALOG,
    USER_INDICATOR
};

constexpr const std::array SensorTypeNames = {
    "NONE"sv,
    "PHYSICAL_ANALOG"sv,
    "VIRTUAL_ANALOG"sv,
    "PHYSICAL_INDICATOR"sv,
    "VIRTUAL_INDICATOR"sv,
    "CANBUS_RAW"sv,
    "CANBUS_ANALOG"sv,
    "CANBUS_INDICATOR"sv,
    "USER_ANALOG"sv,
    "USER_INDICATOR"sv
};

inline const char* GetSensorTypeName(SensorType type) {
    return SensorTypeNames[std::to_underlying(type)].data();
}

inline SensorType GetSensorType(const std::string& name) {
    for(size_t i = 0; i < size(SensorTypeNames); ++i)
        if(SensorTypeNames[i] == name)
            return static_cast<SensorType>(i);

    throw std::runtime_error("Invalid sensor type.");
}

} // namespace eerie_leap::domain::sensor_domain::models
