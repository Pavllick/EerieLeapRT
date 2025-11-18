#include <utility>

#include "utilities/cbor/cbor_helpers.hpp"
#include "utilities/voltage_interpolator/linear_voltage_interpolator.hpp"
#include "utilities/voltage_interpolator/cubic_spline_voltage_interpolator.hpp"
#include "domain/sensor_domain/utilities/sensors_order_resolver.h"
#include "sensor_validator.h"

#include "sensors_cbor_parser.h"

namespace eerie_leap::domain::sensor_domain::utilities::parsers {

using namespace eerie_leap::utilities::cbor;
using namespace eerie_leap::utilities::voltage_interpolator;
using namespace eerie_leap::domain::sensor_domain::utilities;

SensorsCborParser::SensorsCborParser(std::shared_ptr<MathParserService> math_parser_service)
    : math_parser_service_(std::move(math_parser_service)) {}

ext_unique_ptr<CborSensorsConfig> SensorsCborParser::Serialize(
    const std::vector<std::shared_ptr<Sensor>>& sensors,
    uint32_t gpio_channel_count,
    uint32_t adc_channel_count) {

    auto sensors_config = make_unique_ext<CborSensorsConfig>();

    SensorsOrderResolver order_resolver;

    for(const auto& sensor : sensors) {
        // NOTE: UpdateConnectionString() must be called before validation
        sensor->configuration.UpdateConnectionString();
        SensorValidator::Validate(*sensor, gpio_channel_count, adc_channel_count);

        auto sensor_config = make_shared_ext<CborSensorConfig>();

        sensor_config->id = CborHelpers::ToZcborString(&sensor->id);
        sensor_config->configuration.type = std::to_underlying(sensor->configuration.type);

        if(sensor->configuration.channel.has_value()) {
            sensor_config->configuration.channel_present = true;
            sensor_config->configuration.channel = sensor->configuration.channel.value();
        } else {
            sensor_config->configuration.channel_present = false;
        }

        sensor_config->configuration.connection_string = CborHelpers::ToZcborString(&sensor->configuration.connection_string);

        sensor_config->configuration.sampling_rate_ms = sensor->configuration.sampling_rate_ms;

        auto interpolation_method = sensor->configuration.voltage_interpolator != nullptr
            ? sensor->configuration.voltage_interpolator->GetInterpolationMethod()
            : InterpolationMethod::NONE;

        sensor_config->configuration.interpolation_method = static_cast<uint32_t>(interpolation_method);
        if(interpolation_method != InterpolationMethod::NONE) {
            sensor_config->configuration.calibration_table_present = true;

            auto& calibration_table = *sensor->configuration.voltage_interpolator->GetCalibrationTable();

            std::ranges::sort(
                calibration_table,
                [](const CalibrationData& a, const CalibrationData& b) {
                    return a.voltage < b.voltage;
                });

            for(const auto& calibration_data : calibration_table) {
                sensor_config->configuration.calibration_table.float32float.push_back({
                    .float32float_key = calibration_data.voltage,
                    .float32float = calibration_data.value});
            }
        } else {
            sensor_config->configuration.calibration_table_present = false;
        }

        if(sensor->configuration.expression_evaluator != nullptr) {
            sensor_config->configuration.expression_present = true;
            sensor_config->configuration.expression = CborHelpers::ToZcborString(&sensor->configuration.expression_evaluator->GetRawExpression());
        } else {
            sensor_config->configuration.expression_present = false;
        }

        sensor_config->metadata.unit = CborHelpers::ToZcborString(&sensor->metadata.unit);
        sensor_config->metadata.name = CborHelpers::ToZcborString(&sensor->metadata.name);

        sensor_config->metadata.description = CborHelpers::ToZcborString(&sensor->metadata.description);

        sensors_config->CborSensorConfig_m.push_back(*sensor_config);

        order_resolver.AddSensor(sensor);
    }

    // Validate dependencies
    order_resolver.GetProcessingOrder();

    return sensors_config;
}

std::vector<std::shared_ptr<Sensor>> SensorsCborParser::Deserialize(
    const CborSensorsConfig& sensors_config,
    size_t gpio_channel_count,
    size_t adc_channel_count) {

    std::vector<std::shared_ptr<Sensor>> sensors;

    for(const auto& sensor_config : sensors_config.CborSensorConfig_m) {
        std::shared_ptr<Sensor> sensor = make_shared_ext<Sensor>(CborHelpers::ToStdString(sensor_config.id));

        sensor->configuration.type = static_cast<SensorType>(sensor_config.configuration.type);

        if(sensor_config.configuration.channel_present)
            sensor->configuration.channel = sensor_config.configuration.channel;
        else
            sensor->configuration.channel = std::nullopt;

        sensor->configuration.connection_string = CborHelpers::ToStdString(sensor_config.configuration.connection_string);
        sensor->configuration.UnwrapConnectionString();

        sensor->configuration.sampling_rate_ms = sensor_config.configuration.sampling_rate_ms;

        auto interpolation_method = static_cast<InterpolationMethod>(sensor_config.configuration.interpolation_method);
        if(interpolation_method != InterpolationMethod::NONE && sensor_config.configuration.calibration_table_present) {
            std::vector<CalibrationData> calibration_table;
            calibration_table.reserve(sensor_config.configuration.calibration_table.float32float.size());
            for(const auto& calibration_data : sensor_config.configuration.calibration_table.float32float) {
                calibration_table.push_back({
                    .voltage = calibration_data.float32float_key,
                    .value = calibration_data.float32float});
            }

            auto calibration_table_ptr = make_shared_ext<std::vector<CalibrationData>>(calibration_table);

            switch(interpolation_method) {
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

        if(sensor_config.configuration.expression_present)
            sensor->configuration.expression_evaluator = make_unique_ext<ExpressionEvaluator>(
                math_parser_service_,
                CborHelpers::ToStdString(sensor_config.configuration.expression));
        else
            sensor->configuration.expression_evaluator = nullptr;

        sensor->metadata.unit = CborHelpers::ToStdString(sensor_config.metadata.unit);
        sensor->metadata.name = CborHelpers::ToStdString(sensor_config.metadata.name);

        sensor->metadata.description = CborHelpers::ToStdString(sensor_config.metadata.description);

        SensorValidator::Validate(*sensor, gpio_channel_count, adc_channel_count);

        sensors.push_back(sensor);
    }

    return sensors;
}

} // namespace eerie_leap::domain::sensor_domain::utilities::parsers
