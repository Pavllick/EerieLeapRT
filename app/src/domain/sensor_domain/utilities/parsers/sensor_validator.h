#pragma once

#include "domain/sensor_domain/models/sensor.h"

namespace eerie_leap::domain::sensor_domain::utilities::parsers {

using namespace eerie_leap::domain::sensor_domain::models;

class SensorValidator {
public:
    static void ValidateId(const std::string& id);
    static void ValidateType(const SensorConfiguration& sensor_configuration);
    static void ValidateChannel(
        const SensorConfiguration& sensor_configuration,
        uint32_t gpio_channel_count,
        uint32_t adc_channel_count);
    static void ValidateConnectionString(const SensorConfiguration& sensor_configuration);
    static void ValidateSamplingRateMs(const SensorConfiguration& sensor_configuration);
    static void ValidateInterpolationMethod(const SensorConfiguration& sensor_configuration);
    static void ValidateExpression(const SensorConfiguration& sensor_configuration);
    static void Validate(
        const Sensor& sensor,
        uint32_t gpio_channel_count,
        uint32_t adc_channel_count);
};

} // namespace eerie_leap::domain::sensor_domain::utilities::parsers
