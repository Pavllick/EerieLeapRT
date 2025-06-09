#pragma once

#include <cstdint>
#include <string>
#include <stdexcept>

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

    inline InterpolationMethod GetInterpolationMethod(const std::string& name) {
        for(size_t i = 0; i < size(InterpolationMethodNames); ++i)
            if(InterpolationMethodNames[i] == name)
                return static_cast<InterpolationMethod>(i);

        throw std::runtime_error("Invalid interpolation method type.");
    }

} // namespace eerie_leap::domain::sensor_domain::utilities::voltage_interpolator
