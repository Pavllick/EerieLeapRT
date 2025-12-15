#include "sensor_validator.h"

namespace eerie_leap::domain::sensor_domain::configuration::parsers {

static void InvalidSensorConfiguration(std::string_view sensor_id, std::string_view message) {
    throw std::invalid_argument(
        "Invalid Sensor configuration. Sensor ID: "
        + std::string(sensor_id)
        + ". "
        + std::string(message));
}

static void InvalidMetadataConfiguration(std::string_view sensor_id, std::string_view message) {
    throw std::invalid_argument(
        "Invalid Sensor metadata configuration. Sensor ID: "
        + std::string(sensor_id)
        + ". "
        + std::string(message));
}

void SensorValidator::Validate(
    const std::vector<std::shared_ptr<Sensor>>& sensors,
    IFsService* sd_fs_service,
    uint32_t gpio_channel_count,
    uint32_t adc_channel_count) {

    ValidateId(sensors);
    ValidateMetadata(sensors);
    ValidateSensorConfiguration(sensors, sd_fs_service, gpio_channel_count, adc_channel_count);
}

void SensorValidator::ValidateId(const std::vector<std::shared_ptr<Sensor>>& sensors) {
    for(const auto& sensor : sensors) {
        if(sensor->id.empty())
            InvalidSensorConfiguration("", "Sensor ID cannot be empty.");

        static constexpr std::string_view valid_symbols = "_";

        if(!std::isalpha(sensor->id[0]) && valid_symbols.find(sensor->id[0]) == std::string_view::npos)
            InvalidSensorConfiguration("", "Sensor ID must start with a letter or an underscore.");

        if(!std::ranges::all_of(sensor->id, [&valid_symbols](char c) {
            return std::isalnum(c) || valid_symbols.find(c) != std::string_view::npos;})) {

            InvalidSensorConfiguration("", "Sensor ID must contain only letters, digits, and underscores.");
        }
    }
}

void SensorValidator::ValidateMetadata(const std::vector<std::shared_ptr<Sensor>>& sensors) {
    for(const auto& sensor : sensors) {
        ValidateName(sensor->metadata.name);
        ValidateUnit(sensor->metadata.unit);
        ValidateDescription(sensor->metadata.description);
    }
}

void SensorValidator::ValidateName(const std::pmr::string& name) {
    if(name.size() > 32)
        InvalidMetadataConfiguration("", "Sensor name must be less than 32 characters.");
}

void SensorValidator::ValidateUnit(const std::pmr::string& unit) {
    if(unit.size() > 32)
        InvalidMetadataConfiguration("", "Sensor unit must be less than 32 characters.");
}

void SensorValidator::ValidateDescription(const std::pmr::string& description) {
    if(description.size() > 128)
        InvalidMetadataConfiguration("", "Sensor description must be less than 128 characters.");
}

void SensorValidator::ValidateSensorConfiguration(
    const std::vector<std::shared_ptr<Sensor>>& sensors,
    IFsService* sd_fs_service,
    uint32_t gpio_channel_count,
    uint32_t adc_channel_count) {

    for(const auto& sensor : sensors) {
        ValidateType(sensor->id, sensor->configuration);
        ValidateChannel(sensor->id, sensor->configuration, gpio_channel_count, adc_channel_count);
        ValidateConnectionString(sensor->id, sensor->configuration);
        ValidateScriptPath(sensor->id, sensor->configuration, sd_fs_service);
        ValidateSamplingRateMs(sensor->id, sensor->configuration);
        ValidateInterpolationMethod(sensor->id, sensor->configuration);
        ValidateExpression(sensor->id, sensor->configuration);
    }
}

void SensorValidator::ValidateType(std::string_view sensor_id, const SensorConfiguration& sensor_configuration) {
    if(sensor_configuration.type != SensorType::PHYSICAL_ANALOG
        && sensor_configuration.type != SensorType::VIRTUAL_ANALOG
        && sensor_configuration.type != SensorType::PHYSICAL_INDICATOR
        && sensor_configuration.type != SensorType::VIRTUAL_INDICATOR
        && sensor_configuration.type != SensorType::CANBUS_RAW
        && sensor_configuration.type != SensorType::CANBUS_ANALOG
        && sensor_configuration.type != SensorType::CANBUS_INDICATOR
        && sensor_configuration.type != SensorType::USER_ANALOG
        && sensor_configuration.type != SensorType::USER_INDICATOR) {

            InvalidSensorConfiguration(sensor_id, "Invalid sensor type.");
        }
}

void SensorValidator::ValidateChannel(
    std::string_view sensor_id,
    const SensorConfiguration& sensor_configuration,
    uint32_t gpio_channel_count,
    uint32_t adc_channel_count) {

    if(sensor_configuration.type != SensorType::PHYSICAL_ANALOG
        && sensor_configuration.type != SensorType::PHYSICAL_INDICATOR
        && sensor_configuration.channel.has_value()) {

        InvalidSensorConfiguration(sensor_id, "Channel value is not supported for this sensor type.");
    }

    if((sensor_configuration.type == SensorType::PHYSICAL_INDICATOR || sensor_configuration.type == SensorType::PHYSICAL_ANALOG)
        && !sensor_configuration.channel.has_value()) {

        InvalidSensorConfiguration(sensor_id, "Sensor channel is not set.");
    }

    if(!sensor_configuration.channel.has_value())
        return;

    uint32_t channel_count = 0;
    if(sensor_configuration.type == SensorType::PHYSICAL_INDICATOR)
        channel_count = gpio_channel_count;
    else if(sensor_configuration.type == SensorType::PHYSICAL_ANALOG)
        channel_count = adc_channel_count;

    if(sensor_configuration.channel.value() >= channel_count)
        InvalidSensorConfiguration(sensor_id, "Channel value is out of range.");
}

void SensorValidator::ValidateConnectionString(std::string_view sensor_id, const SensorConfiguration& sensor_configuration) {
    if(sensor_configuration.type != SensorType::CANBUS_RAW
        && sensor_configuration.type != SensorType::CANBUS_ANALOG
        && sensor_configuration.type != SensorType::CANBUS_INDICATOR) {

        if(!sensor_configuration.connection_string.empty())
            InvalidSensorConfiguration(sensor_id, "Connection string is not supported for this sensor type.");

        return;
    }

    if(sensor_configuration.connection_string.empty())
        InvalidSensorConfiguration(sensor_id, "Connection string cannot be empty.");

    if((sensor_configuration.type == SensorType::CANBUS_ANALOG
        || sensor_configuration.type == SensorType::CANBUS_INDICATOR)
        && sensor_configuration.canbus_source->signal_name.empty()) {

        InvalidSensorConfiguration(sensor_id, "Sensor must have CAN bus signal name.");
    }
}

void SensorValidator::ValidateScriptPath(std::string_view sensor_id, const SensorConfiguration& sensor_configuration, IFsService* sd_fs_service) {
    if(sd_fs_service == nullptr)
        return;

    if(!sensor_configuration.script_path.empty()) {
        if(!sd_fs_service->Exists(sensor_configuration.script_path))
            InvalidSensorConfiguration(sensor_id, "Invalid sensor script path.");
    }
}

// TODO: Review sampling_rate_ms == 0 case
void SensorValidator::ValidateSamplingRateMs(std::string_view sensor_id, const SensorConfiguration& sensor_configuration) {
    if(sensor_configuration.sampling_rate_ms < 1)
        InvalidSensorConfiguration(sensor_id, "Invalid sampling rate value.");
}

void SensorValidator::ValidateInterpolationMethod(std::string_view sensor_id, const SensorConfiguration& sensor_configuration) {
    if(sensor_configuration.type != SensorType::PHYSICAL_ANALOG && sensor_configuration.voltage_interpolator != nullptr)
        InvalidSensorConfiguration(sensor_id, "Sensor does not support interpolation.");

    if(sensor_configuration.type != SensorType::PHYSICAL_ANALOG)
        return;

    if(sensor_configuration.voltage_interpolator == nullptr)
        InvalidSensorConfiguration(sensor_id, "Sensor must have interpolation method.");

    const auto& calibration_table = *sensor_configuration.voltage_interpolator->GetCalibrationTable();

    if(calibration_table.size() < 2)
        InvalidSensorConfiguration(sensor_id, "Calibration table must have at least 2 points.");
}

void SensorValidator::ValidateExpression(std::string_view sensor_id, const SensorConfiguration& sensor_configuration) {
    if(sensor_configuration.type == SensorType::VIRTUAL_ANALOG || sensor_configuration.type == SensorType::VIRTUAL_INDICATOR) {
        if(sensor_configuration.expression_evaluator == nullptr)
            InvalidSensorConfiguration(sensor_id, "Sensor must have expression evaluator.");
    }

    if(sensor_configuration.type == SensorType::CANBUS_RAW && sensor_configuration.expression_evaluator != nullptr)
        InvalidSensorConfiguration(sensor_id, "Sensor does not support expression evaluator.");
}

} // namespace eerie_leap::domain::sensor_domain::configuration::parsers
