#pragma once

#include <cstdint>
#include <array>
#include <stdexcept>
#include <utility>
#include <string_view>

namespace eerie_leap::subsys::canbus {

using namespace std::string_view_literals;

enum class CanbusType : uint8_t {
    NONE,
    CLASSICAL_CAN,
    CANFD
};

constexpr const std::array CanbusTypeNames = {
    "NONE"sv,
    "CLASSICAL_CAN"sv,
    "CANFD"sv
};

inline const char* GetCanbusTypeName(CanbusType type) {
    return CanbusTypeNames[std::to_underlying(type)].data();
}

inline CanbusType GetCanbusType(const std::string& name) {
    for(size_t i = 0; i < size(CanbusTypeNames); ++i)
        if(CanbusTypeNames[i] == name)
            return static_cast<CanbusType>(i);

    throw std::runtime_error("Invalid canbus type.");
}

}  // namespace eerie_leap::subsys::canbus
