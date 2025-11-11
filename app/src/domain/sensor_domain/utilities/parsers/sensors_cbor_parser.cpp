#include <utility>

#include "utilities/cbor/cbor_helpers.hpp"
#include "utilities/voltage_interpolator/linear_voltage_interpolator.hpp"
#include "utilities/voltage_interpolator/cubic_spline_voltage_interpolator.hpp"
#include "domain/sensor_domain/utilities/sensors_order_resolver.h"
#include "domain/sensor_domain/utilities/sensor_helpers.h"

#include "sensors_cbor_parser.h"

namespace eerie_leap::domain::sensor_domain::utilities::parsers {

using namespace eerie_leap::utilities::cbor;
using namespace eerie_leap::domain::sensor_domain::utilities;
using namespace eerie_leap::utilities::voltage_interpolator;

SensorsCborParser::SensorsCborParser(std::shared_ptr<MathParserService> math_parser_service)
    : math_parser_service_(std::move(math_parser_service)) {}

ext_unique_ptr<SensorsConfig> SensorsCborParser::Serialize(
    const std::vector<std::shared_ptr<Sensor>>& sensors,
    size_t gpio_channel_count,
    size_t adc_channel_count) {

    auto sensors_config = make_unique_ext<SensorsConfig>();
    memset(sensors_config.get(), 0, sizeof(SensorsConfig));

    SensorsOrderResolver resolver;

    for(size_t i = 0; i < sensors.size(); ++i) {
        const auto& sensor = sensors.at(i);

        ValidateSensorType(sensor->configuration);

        auto sensor_config = make_shared_ext<SensorConfig>();
        memset(sensor_config.get(), 0, sizeof(SensorConfig));

        if(!SensorHelpers::IsIdValid(sensor->id))
            throw std::runtime_error("Invalid sensor ID.");

        sensor_config->id = CborHelpers::ToZcborString(&sensor->id);
        sensor_config->configuration.type = std::to_underlying(sensor->configuration.type);

        if(sensor->configuration.channel.has_value()) {
            int channel_count = -1;

            if(sensor->configuration.type == SensorType::PHYSICAL_INDICATOR)
                channel_count = gpio_channel_count;
            else if(sensor->configuration.type == SensorType::PHYSICAL_ANALOG)
                channel_count = adc_channel_count;
            else
                throw std::runtime_error("Invalid sensor type.");

            if(sensor->configuration.channel.value() < 0 || sensor->configuration.channel.value() >= channel_count)
                throw std::runtime_error("Invalid channel value.");

            sensor_config->configuration.channel_present = true;
            sensor_config->configuration.channel = sensor->configuration.channel.value();
        } else {
            sensor_config->configuration.channel_present = false;
        }

        sensor->configuration.UpdateConnectionString();
        sensor_config->configuration.connection_string = CborHelpers::ToZcborString(&sensor->configuration.connection_string);

        sensor_config->configuration.sampling_rate_ms = sensor->configuration.sampling_rate_ms;

        auto interpolation_method = sensor->configuration.voltage_interpolator != nullptr
            ? sensor->configuration.voltage_interpolator->GetInterpolationMethod()
            : InterpolationMethod::NONE;

        sensor_config->configuration.interpolation_method = static_cast<uint32_t>(interpolation_method);
        if(interpolation_method != InterpolationMethod::NONE) {
            sensor_config->configuration.calibration_table_present = true;

            auto& calibration_table = *sensor->configuration.voltage_interpolator->GetCalibrationTable();
            sensor_config->configuration.calibration_table.float32float_count = calibration_table.size();

            if(calibration_table.size() < 2)
                throw std::runtime_error("Calibration table must have at least 2 points.");

            std::ranges::sort(
                calibration_table,
                [](const CalibrationData& a, const CalibrationData& b) {
                    return a.voltage < b.voltage;
                });

            for(size_t j = 0; j < calibration_table.size(); ++j) {
                const auto& calibration_data = calibration_table[j];
                sensor_config->configuration.calibration_table.float32float[j].float32float_key = calibration_data.voltage;
                sensor_config->configuration.calibration_table.float32float[j].float32float = calibration_data.value;
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

        sensors_config->SensorConfig_m[i] = *sensor_config;
        sensors_config->SensorConfig_m_count++;

        resolver.AddSensor(sensor);
    }

    // Validate dependencies
    resolver.GetProcessingOrder();

    return sensors_config;
}

std::vector<std::shared_ptr<Sensor>> SensorsCborParser::Deserialize(const SensorsConfig& sensors_config) {
    std::vector<std::shared_ptr<Sensor>> sensors;

    for(size_t i = 0; i < sensors_config.SensorConfig_m_count; ++i) {
        const auto& sensor_config = sensors_config.SensorConfig_m[i];
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
            for(size_t j = 0; j < sensor_config.configuration.calibration_table.float32float_count; ++j) {
                const auto& calibration_data = sensor_config.configuration.calibration_table.float32float[j];

                calibration_table.push_back({
                    .voltage = calibration_data.float32float_key,
                    .value = calibration_data.float32float});
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

        if(sensor_config.configuration.expression_present)
            sensor->configuration.expression_evaluator = make_unique_ext<ExpressionEvaluator>(
                math_parser_service_,
                CborHelpers::ToStdString(sensor_config.configuration.expression));
        else
            sensor->configuration.expression_evaluator = nullptr;

        sensor->metadata.unit = CborHelpers::ToStdString(sensor_config.metadata.unit);
        sensor->metadata.name = CborHelpers::ToStdString(sensor_config.metadata.name);

        sensor->metadata.description = CborHelpers::ToStdString(sensor_config.metadata.description);

        sensors.push_back(sensor);
    }

    return sensors;
}

void SensorsCborParser::ValidateSensorType(const SensorConfiguration& sensor_configuration) {
    if(sensor_configuration.type == SensorType::PHYSICAL_ANALOG) {
        if(!sensor_configuration.channel.has_value())
            throw std::runtime_error("Physical Analog sensor must have channel.");

        if(sensor_configuration.voltage_interpolator == nullptr)
            throw std::runtime_error("Physical Analog sensor must have interpolation method.");
    } else if(sensor_configuration.type == SensorType::PHYSICAL_INDICATOR) {
        if(!sensor_configuration.channel.has_value())
            throw std::runtime_error("Physical Indicator sensor must have channel.");

        if(sensor_configuration.voltage_interpolator != nullptr)
            throw std::runtime_error("Physical Indicator sensor must not have interpolation method.");
    } else if(sensor_configuration.type == SensorType::CANBUS_RAW) {
        if(sensor_configuration.canbus_source == nullptr)
            throw std::runtime_error("Canbus sensor must have source.");

        if(sensor_configuration.voltage_interpolator != nullptr)
            throw std::runtime_error("Canbus sensor must not have interpolation method.");

        if(sensor_configuration.expression_evaluator != nullptr)
            throw std::runtime_error("Canbus Raw sensor must not have math expression.");
    } else if(sensor_configuration.type == SensorType::CANBUS_ANALOG || sensor_configuration.type == SensorType::CANBUS_INDICATOR) {
        if(sensor_configuration.canbus_source == nullptr)
            throw std::runtime_error("Canbus sensor must have source.");

        if(sensor_configuration.canbus_source->signal_name.empty())
            throw std::runtime_error("Canbus sensor must have signal name.");

        if(sensor_configuration.voltage_interpolator != nullptr)
            throw std::runtime_error("Canbus sensor must not have interpolation method.");
    } else if(sensor_configuration.type == SensorType::VIRTUAL_ANALOG || sensor_configuration.type == SensorType::VIRTUAL_INDICATOR) {
        if(sensor_configuration.expression_evaluator == nullptr)
            throw std::runtime_error("Virtual sensor must have expression evaluator.");

        if(sensor_configuration.voltage_interpolator != nullptr)
            throw std::runtime_error("Virtual sensor must not have interpolation method.");
    }
}

} // namespace eerie_leap::domain::sensor_domain::utilities::parsers
