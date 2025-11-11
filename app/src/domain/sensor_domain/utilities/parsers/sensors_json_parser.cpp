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

ext_unique_ptr<ExtVector> SensorsJsonParser::Serialize(
    const std::vector<std::shared_ptr<Sensor>>& sensors,
    uint32_t gpio_channel_count,
    uint32_t adc_channel_count) {

    SensorsJsonDto sensors_json;
    memset(&sensors_json, 0, sizeof(SensorsJsonDto));

    SensorsOrderResolver order_resolver;

    for(size_t i = 0; i < sensors.size(); ++i) {
        const auto& sensor = sensors.at(i);

        // NOTE: UpdateConnectionString() must be called before validation
        sensor->configuration.UpdateConnectionString();
        SensorValidator::ValidateSensor(*sensor, gpio_channel_count, adc_channel_count);

        SensorJsonDto sensor_json;
        memset(&sensor_json, 0, sizeof(SensorJsonDto));

        sensor_json.id = sensor->id.c_str();
        sensor_json.configuration.type = GetSensorTypeName(sensor->configuration.type);

        if(sensor->configuration.channel.has_value())
            sensor_json.configuration.channel = sensor->configuration.channel.value();

        sensor_json.configuration.connection_string = sensor->configuration.connection_string.c_str();

        sensor_json.configuration.sampling_rate_ms = sensor->configuration.sampling_rate_ms;

        auto interpolation_method = sensor->configuration.voltage_interpolator != nullptr
            ? sensor->configuration.voltage_interpolator->GetInterpolationMethod()
            : InterpolationMethod::NONE;

        sensor_json.configuration.interpolation_method = GetInterpolationMethodName(interpolation_method);
        if(interpolation_method != InterpolationMethod::NONE) {
            sensor_json.configuration.calibration_table_len = sensor->configuration.voltage_interpolator->GetCalibrationTable()->size();

            auto& calibration_table = *sensor->configuration.voltage_interpolator->GetCalibrationTable();

            std::ranges::sort(
                calibration_table,
                [](const CalibrationData& a, const CalibrationData& b) {
                    return a.voltage < b.voltage;
                });

            for(size_t j = 0; j < calibration_table.size(); ++j) {
                const auto& calibration_data = calibration_table[j];
                sensor_json.configuration.calibration_table[j].voltage = calibration_data.voltage;
                sensor_json.configuration.calibration_table[j].value = calibration_data.value;
                sensor_json.configuration.calibration_table_len++;
            }
        }

        if(sensor->configuration.expression_evaluator != nullptr)
            sensor_json.configuration.expression = sensor->configuration.expression_evaluator->GetRawExpression().c_str();

        sensor_json.metadata.unit = sensor->metadata.unit.c_str();
        sensor_json.metadata.name = sensor->metadata.name.c_str();
        sensor_json.metadata.description = sensor->metadata.description.c_str();

        sensors_json.sensors[i] = sensor_json;
        sensors_json.sensors_len++;

        order_resolver.AddSensor(sensor);
    }

    // Validate dependencies
    order_resolver.GetProcessingOrder();

    auto buf_size = json_calc_encoded_len(sensors_descr, ARRAY_SIZE(sensors_descr), &sensors_json);
    if(buf_size < 0)
        throw std::runtime_error("Failed to calculate buffer size.");

    // Add 1 for null terminator
    auto buffer = make_unique_ext<ExtVector>(buf_size + 1);

    int res = json_obj_encode_buf(sensors_descr, ARRAY_SIZE(sensors_descr), &sensors_json, (char*)buffer->data(), buffer->size());
    if(res != 0)
        throw std::runtime_error("Failed to serialize sensors.");

    return buffer;
}

std::vector<std::shared_ptr<Sensor>> SensorsJsonParser::Deserialize(
    const std::span<const uint8_t>& json,
    uint32_t gpio_channel_count,
    uint32_t adc_channel_count) {

	SensorsJsonDto sensors_json;
	const int expected_return_code = BIT_MASK(ARRAY_SIZE(sensors_descr));

	int ret = json_obj_parse((char*)json.data(), json.size(), sensors_descr, ARRAY_SIZE(sensors_descr), &sensors_json);
	if(ret != expected_return_code)
        throw std::runtime_error("Invalid JSON payload.");

    std::vector<std::shared_ptr<Sensor>> sensors;

    for(size_t i = 0; i < sensors_json.sensors_len; ++i) {
        auto sensor = make_shared_ext<Sensor>(sensors_json.sensors[i].id);

        sensor->metadata.name = sensors_json.sensors[i].metadata.name;
        sensor->metadata.unit = sensors_json.sensors[i].metadata.unit == nullptr ? "" : sensors_json.sensors[i].metadata.unit;
        sensor->metadata.description = sensors_json.sensors[i].metadata.description == nullptr ? "" : sensors_json.sensors[i].metadata.description;

        sensor->configuration.type = GetSensorType(sensors_json.sensors[i].configuration.type);
        sensor->configuration.channel = std::nullopt;
        if(sensor->configuration.type == SensorType::PHYSICAL_ANALOG || sensor->configuration.type == SensorType::PHYSICAL_INDICATOR)
            sensor->configuration.channel = sensors_json.sensors[i].configuration.channel;
        sensor->configuration.sampling_rate_ms = sensors_json.sensors[i].configuration.sampling_rate_ms;

        sensor->configuration.connection_string = sensors_json.sensors[i].configuration.connection_string;
        sensor->configuration.UnwrapConnectionString();

        InterpolationMethod interpolation_method = InterpolationMethod::NONE;
        if(sensor->configuration.type == SensorType::PHYSICAL_ANALOG)
            interpolation_method = GetInterpolationMethod(sensors_json.sensors[i].configuration.interpolation_method);

        if(interpolation_method != InterpolationMethod::NONE && sensors_json.sensors[i].configuration.calibration_table_len > 0) {
            std::vector<CalibrationData> calibration_table;
            for(size_t j = 0; j < sensors_json.sensors[i].configuration.calibration_table_len; ++j) {
                const auto& calibration_data = sensors_json.sensors[i].configuration.calibration_table[j];

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

        if(sensors_json.sensors[i].configuration.expression != nullptr && strcmp(sensors_json.sensors[i].configuration.expression, "") != 0) {
            sensor->configuration.expression_evaluator = make_unique_ext<ExpressionEvaluator>(
                math_parser_service_,
                std::string(sensors_json.sensors[i].configuration.expression));
        } else {
            sensor->configuration.expression_evaluator = nullptr;
        }

        SensorValidator::ValidateSensor(*sensor, gpio_channel_count, adc_channel_count);

        sensors.push_back(sensor);
    }

    return sensors;
}

} // eerie_leap::domain::sensor_domain::utilities::parsers
