#include "sensor_validator.h"

namespace eerie_leap::domain::sensor_domain::configuration::parsers {

void SensorValidator::ValidateId(const std::string& id) {
    if(id.empty())
        throw std::runtime_error("Sensor ID cannot be empty.");

    static constexpr std::string_view valid_symbols = "_";

    if(!std::isalpha(id[0]) && valid_symbols.find(id[0]) == std::string_view::npos)
        throw std::runtime_error("Sensor ID must start with a letter or an underscore.");

    if(!std::ranges::all_of(id, [&valid_symbols](char c) {
        return std::isalnum(c) || valid_symbols.find(c) != std::string_view::npos;})) {

        throw std::runtime_error("Sensor ID must contain only letters, digits, and underscores.");
    }
}

void SensorValidator::ValidateType(const SensorConfiguration& sensor_configuration) {
    if(sensor_configuration.type != SensorType::PHYSICAL_ANALOG
        && sensor_configuration.type != SensorType::VIRTUAL_ANALOG
        && sensor_configuration.type != SensorType::PHYSICAL_INDICATOR
        && sensor_configuration.type != SensorType::VIRTUAL_INDICATOR
        && sensor_configuration.type != SensorType::CANBUS_RAW
        && sensor_configuration.type != SensorType::CANBUS_ANALOG
        && sensor_configuration.type != SensorType::CANBUS_INDICATOR
        && sensor_configuration.type != SensorType::USER_ANALOG
        && sensor_configuration.type != SensorType::USER_INDICATOR) {

            throw std::runtime_error("Invalid sensor type.");
        }
}

void SensorValidator::ValidateChannel(
    const SensorConfiguration& sensor_configuration,
    uint32_t gpio_channel_count,
    uint32_t adc_channel_count) {

    if(sensor_configuration.type != SensorType::PHYSICAL_ANALOG
        && sensor_configuration.type != SensorType::PHYSICAL_INDICATOR
        && sensor_configuration.channel.has_value()) {

        throw std::runtime_error("Invalid channel value.");
    }

    if(!sensor_configuration.channel.has_value())
        return;

    uint32_t channel_count = 0;

    if(sensor_configuration.type == SensorType::PHYSICAL_INDICATOR)
        channel_count = gpio_channel_count;
    else if(sensor_configuration.type == SensorType::PHYSICAL_ANALOG)
        channel_count = adc_channel_count;
    else
        throw std::runtime_error("Invalid sensor type.");

    if(sensor_configuration.channel.value() < 0 || sensor_configuration.channel.value() >= channel_count)
        throw std::runtime_error("Invalid channel value.");
}

void SensorValidator::ValidateConnectionString(const SensorConfiguration& sensor_configuration) {
    bool is_invalid = false;

    if(sensor_configuration.type == SensorType::CANBUS_RAW
        || sensor_configuration.type == SensorType::CANBUS_ANALOG
        || sensor_configuration.type == SensorType::CANBUS_INDICATOR) {

        if(sensor_configuration.connection_string.empty() || sensor_configuration.canbus_source == nullptr)
            is_invalid = true;

        if((sensor_configuration.type == SensorType::CANBUS_ANALOG
            || sensor_configuration.type == SensorType::CANBUS_INDICATOR)
            && sensor_configuration.canbus_source->signal_name.empty()) {

            throw std::runtime_error("Sensor must have CAN bus signal name.");
        }
    } else if(!sensor_configuration.connection_string.empty()) {
        is_invalid = true;
    }

    if(is_invalid)
        throw std::runtime_error("Invalid connection string value.");
}

void SensorValidator::ValidateSamplingRateMs(const SensorConfiguration& sensor_configuration) {
    if(sensor_configuration.sampling_rate_ms < 1)
        throw std::runtime_error("Invalid sampling rate value.");
}

void SensorValidator::ValidateInterpolationMethod(const SensorConfiguration& sensor_configuration) {
    if(sensor_configuration.type != SensorType::PHYSICAL_ANALOG
        && sensor_configuration.voltage_interpolator != nullptr) {

        throw std::runtime_error("Sensor must have interpolation method.");
    } else if(sensor_configuration.voltage_interpolator != nullptr) {
        const auto& calibration_table = *sensor_configuration.voltage_interpolator->GetCalibrationTable();

        if(calibration_table.size() < 2)
            throw std::runtime_error("Calibration table must have at least 2 points.");
    }
}

void SensorValidator::ValidateExpression(const SensorConfiguration& sensor_configuration) {
    if(sensor_configuration.type == SensorType::VIRTUAL_ANALOG
        || sensor_configuration.type == SensorType::VIRTUAL_INDICATOR) {

        if(sensor_configuration.expression_evaluator == nullptr)
            throw std::runtime_error("Sensor must have expression evaluator.");
    } else if(sensor_configuration.type == SensorType::CANBUS_RAW
        && sensor_configuration.expression_evaluator != nullptr) {

        throw std::runtime_error("Sensor does not support expression evaluator.");
    }
}

void SensorValidator::Validate(
    const Sensor& sensor,
    uint32_t gpio_channel_count,
    uint32_t adc_channel_count) {

    ValidateId(sensor.id);
    ValidateChannel(sensor.configuration, gpio_channel_count, adc_channel_count);
    ValidateType(sensor.configuration);
    ValidateConnectionString(sensor.configuration);
    ValidateSamplingRateMs(sensor.configuration);
    ValidateInterpolationMethod(sensor.configuration);
    ValidateExpression(sensor.configuration);
}

} // namespace eerie_leap::domain::sensor_domain::configuration::parsers
