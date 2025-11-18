#include "utilities/voltage_interpolator/interpolation_method.h"
#include "utilities/voltage_interpolator/linear_voltage_interpolator.hpp"
#include "utilities/voltage_interpolator/cubic_spline_voltage_interpolator.hpp"
#include "domain/sensor_domain/utilities/sensors_order_resolver.h"
#include "sensor_validator.h"

#include "sensors_json_parser.h"

namespace eerie_leap::domain::sensor_domain::utilities::parsers {

using namespace eerie_leap::utilities::memory;
using namespace eerie_leap::utilities::voltage_interpolator;
using namespace eerie_leap::domain::sensor_domain::utilities;

SensorsJsonParser::SensorsJsonParser(
    std::shared_ptr<MathParserService> math_parser_service)
    : math_parser_service_(std::move(math_parser_service)) {}

ext_unique_ptr<JsonSensorsConfig> SensorsJsonParser::Serialize(
    const std::vector<std::shared_ptr<Sensor>>& sensors,
    uint32_t gpio_channel_count,
    uint32_t adc_channel_count) {

    auto config = make_unique_ext<JsonSensorsConfig>();

    SensorsOrderResolver order_resolver;

    for(auto& sensor : sensors) {
        // NOTE: UpdateConnectionString() must be called before validation
        sensor->configuration.UpdateConnectionString();
        SensorValidator::Validate(*sensor, gpio_channel_count, adc_channel_count);

        JsonSensorConfig sensor_json;

        sensor_json.id = sensor->id;
        sensor_json.configuration.type = GetSensorTypeName(sensor->configuration.type);

        if(sensor->configuration.channel.has_value())
            sensor_json.configuration.channel = sensor->configuration.channel.value();

        sensor_json.configuration.connection_string = sensor->configuration.connection_string;

        sensor_json.configuration.sampling_rate_ms = sensor->configuration.sampling_rate_ms;

        auto interpolation_method = sensor->configuration.voltage_interpolator != nullptr
            ? sensor->configuration.voltage_interpolator->GetInterpolationMethod()
            : InterpolationMethod::NONE;

        sensor_json.configuration.interpolation_method = GetInterpolationMethodName(interpolation_method);
        if(interpolation_method != InterpolationMethod::NONE) {
            auto& calibration_table = *sensor->configuration.voltage_interpolator->GetCalibrationTable();

            std::ranges::sort(
                calibration_table,
                [](const CalibrationData& a, const CalibrationData& b) {
                    return a.voltage < b.voltage;
                });

            for(auto& calibration_data : calibration_table) {
                sensor_json.configuration.calibration_table.push_back({
                    .voltage = calibration_data.voltage,
                    .value = calibration_data.value});
            }
        }

        if(sensor->configuration.expression_evaluator != nullptr)
            sensor_json.configuration.expression = sensor->configuration.expression_evaluator->GetRawExpression();

        sensor_json.metadata.unit = sensor->metadata.unit;
        sensor_json.metadata.name = sensor->metadata.name;
        sensor_json.metadata.description = sensor->metadata.description;

        config->sensors.push_back(sensor_json);

        order_resolver.AddSensor(sensor);
    }

    // Validate dependencies
    order_resolver.GetProcessingOrder();

    return config;
}

std::vector<std::shared_ptr<Sensor>> SensorsJsonParser::Deserialize(
    const JsonSensorsConfig& config,
    uint32_t gpio_channel_count,
    uint32_t adc_channel_count) {

    std::vector<std::shared_ptr<Sensor>> sensors;

    for(auto& sensor_config : config.sensors) {
        auto sensor = make_shared_ext<Sensor>(sensor_config.id);

        sensor->metadata.name = sensor_config.metadata.name;
        sensor->metadata.unit = sensor_config.metadata.unit;
        sensor->metadata.description = sensor_config.metadata.description;

        sensor->configuration.type = GetSensorType(sensor_config.configuration.type);
        sensor->configuration.channel = std::nullopt;
        if(sensor->configuration.type == SensorType::PHYSICAL_ANALOG || sensor->configuration.type == SensorType::PHYSICAL_INDICATOR)
            sensor->configuration.channel = sensor_config.configuration.channel;
        sensor->configuration.sampling_rate_ms = sensor_config.configuration.sampling_rate_ms;

        sensor->configuration.connection_string = sensor_config.configuration.connection_string;
        sensor->configuration.UnwrapConnectionString();

        InterpolationMethod interpolation_method = InterpolationMethod::NONE;
        if(sensor->configuration.type == SensorType::PHYSICAL_ANALOG)
            interpolation_method = GetInterpolationMethod(sensor_config.configuration.interpolation_method);

        if(interpolation_method != InterpolationMethod::NONE && sensor_config.configuration.calibration_table.size() > 0) {
            std::vector<CalibrationData> calibration_table;
            for(auto& calibration_data : sensor_config.configuration.calibration_table) {
                calibration_table.push_back({
                    .voltage = calibration_data.voltage,
                    .value = calibration_data.value});
            }

            auto calibration_table_ptr = make_shared_ext<std::vector<CalibrationData>>(calibration_table);

            switch (interpolation_method) {
            case InterpolationMethod::LINEAR:
                sensor->configuration.voltage_interpolator = make_unique_ext<LinearVoltageInterpolator>(calibration_table_ptr);
                break;

            case InterpolationMethod::CUBIC_SPLINE:
                sensor->configuration.voltage_interpolator = make_unique_ext<CubicSplineVoltageInterpolator>(calibration_table_ptr);
                break;

            default:
                throw std::runtime_error("Sensor uses unsupported interpolation method.");
                break;
            }
        } else {
            sensor->configuration.voltage_interpolator = nullptr;
        }

        if(!sensor_config.configuration.expression.empty()) {
            sensor->configuration.expression_evaluator = make_unique_ext<ExpressionEvaluator>(
                math_parser_service_,
                std::string(sensor_config.configuration.expression));
        } else {
            sensor->configuration.expression_evaluator = nullptr;
        }

        SensorValidator::Validate(*sensor, gpio_channel_count, adc_channel_count);

        sensors.push_back(sensor);
    }

    return sensors;
}

} // eerie_leap::domain::sensor_domain::utilities::parsers
