#pragma once

namespace eerie_leap::domain::sensor_domain::utilities::voltage_interpolator {

    enum class InterpolationMethod {
        NONE,
        LINEAR,
        CUBIC_SPLINE
    };

}