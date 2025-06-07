#pragma once

#include <cstdint>
#include <string>

namespace eerie_leap::domain::sensor_domain::utilities::voltage_interpolator {

    enum class InterpolationMethod : std::uint8_t {
        NONE,
        LINEAR,
        CUBIC_SPLINE
    };

    constexpr const std::string InterpolationMethodNames[] = {
        "NONE",
        "LINEAR",
        "CUBIC_SPLINE"
    };

    inline const std::string& GetInterpolationMethodName(InterpolationMethod method) {
        return InterpolationMethodNames[static_cast<std::uint8_t>(method)];
    }

} // namespace eerie_leap::domain::sensor_domain::utilities::voltage_interpolator
