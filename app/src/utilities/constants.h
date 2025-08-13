#pragma once

namespace eerie_leap::utilities::constants {

    namespace adc {
        static const float ADC_VOLTAGE_MAX = 3.1F;
        static const float SENSOR_VOLTAGE_MAX = 5.0F;

        static const int ADC_CALIBRATION_SAMPLES_COUNT = 2048;
        static const int ADC_CALIBRATION_SAMPLING_RATE_MS = 100;
    }

    namespace fs {
        static const int SD_CHECK_INTERVAL_MS = 1000;
        static const char* SD_DRIVE_NAME = "SD";
        static const char* SD_MOUNT_POINT = "/ext";
    }

} // namespace eerie_leap::utilities::constants
