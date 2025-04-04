#pragma once

namespace eerie_leap::domain::sensor_domain::models {

    enum class ReadingStatus {
        UNINITIALIZED,
        RAW,
        VALIDATED,
        PROCESSED,
        ERROR
    };

}