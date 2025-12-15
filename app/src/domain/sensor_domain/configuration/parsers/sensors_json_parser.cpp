#include "utilities/voltage_interpolator/interpolation_method.h"
#include "utilities/voltage_interpolator/linear_voltage_interpolator.hpp"
#include "utilities/voltage_interpolator/cubic_spline_voltage_interpolator.hpp"
#include "subsys/lua_script/lua_script.h"
#include "domain/sensor_domain/utilities/sensors_order_resolver.h"
#include "sensor_validator.h"

#include "sensors_json_parser.h"

namespace eerie_leap::domain::sensor_domain::configuration::parsers {

using namespace eerie_leap::utilities::memory;
using namespace eerie_leap::utilities::voltage_interpolator;
using namespace eerie_leap::subsys::lua_script;
using namespace eerie_leap::domain::sensor_domain::utilities;

SensorsJsonParser::SensorsJsonParser(std::shared_ptr<IFsService> sd_fs_service)
    : sd_fs_service_(std::move(sd_fs_service)) {}

pmr_unique_ptr<JsonSensorsConfig> SensorsJsonParser::Serialize(
    const std::vector<std::shared_ptr<Sensor>>& sensors,
    uint32_t gpio_channel_count,
    uint32_t adc_channel_count) {

    // NOTE: UpdateConnectionString() must be called before validation
    for(auto& sensor : sensors)
        sensor->configuration.UpdateConnectionString();
    SensorValidator::Validate(sensors, sd_fs_service_.get(), gpio_channel_count, adc_channel_count);

    auto config = make_unique_pmr<JsonSensorsConfig>(Mrm::GetExtPmr());
    SensorsOrderResolver order_resolver;

    for(auto& sensor : sensors) {
        auto sensor_config = make_unique_pmr<JsonSensorConfig>(Mrm::GetExtPmr());

        sensor_config->id = json::string(sensor->id);
        sensor_config->configuration.type = json::string(GetSensorTypeName(sensor->configuration.type));

        if(sensor->configuration.channel.has_value())
            sensor_config->configuration.channel = sensor->configuration.channel.value();

        sensor_config->configuration.connection_string = json::string(sensor->configuration.connection_string);
        sensor_config->configuration.script_path = json::string(sensor->configuration.script_path);
        sensor_config->configuration.sampling_rate_ms = sensor->configuration.sampling_rate_ms;

        auto interpolation_method = sensor->configuration.voltage_interpolator != nullptr
            ? sensor->configuration.voltage_interpolator->GetInterpolationMethod()
            : InterpolationMethod::NONE;

        sensor_config->configuration.interpolation_method = GetInterpolationMethodName(interpolation_method);
        if(interpolation_method != InterpolationMethod::NONE) {
            auto& calibration_table = *sensor->configuration.voltage_interpolator->GetCalibrationTable();

            std::ranges::sort(
                calibration_table,
                [](const CalibrationData& a, const CalibrationData& b) {
                    return a.voltage < b.voltage;
                });

            for(auto& calibration_data : calibration_table) {
                sensor_config->configuration.calibration_table.push_back({
                    .voltage = calibration_data.voltage,
                    .value = calibration_data.value});
            }
        }

        if(sensor->configuration.expression_evaluator != nullptr)
            sensor_config->configuration.expression = json::string(sensor->configuration.expression_evaluator->GetExpression());

        sensor_config->metadata.unit = json::string(sensor->metadata.unit);
        sensor_config->metadata.name = json::string(sensor->metadata.name);
        sensor_config->metadata.description = json::string(sensor->metadata.description);

        config->sensors.push_back(std::move(*sensor_config));

        order_resolver.AddSensor(sensor);
    }

    // Validate dependencies
    order_resolver.GetProcessingOrder();

    return config;
}

std::vector<std::shared_ptr<Sensor>> SensorsJsonParser::Deserialize(
    std::pmr::memory_resource* mr,
    const JsonSensorsConfig& config,
    uint32_t gpio_channel_count,
    uint32_t adc_channel_count) {

    SensorsOrderResolver order_resolver;

    for(const auto& sensor_config : config.sensors) {
        auto sensor = make_shared_pmr<Sensor>(mr, std::string(sensor_config.id));

        sensor->metadata.name = std::string(sensor_config.metadata.name);
        sensor->metadata.unit = std::string(sensor_config.metadata.unit);
        sensor->metadata.description = std::string(sensor_config.metadata.description);

        sensor->configuration.type = GetSensorType(std::string(sensor_config.configuration.type));
        sensor->configuration.channel = std::nullopt;
        if(sensor->configuration.type == SensorType::PHYSICAL_ANALOG || sensor->configuration.type == SensorType::PHYSICAL_INDICATOR)
            sensor->configuration.channel = sensor_config.configuration.channel;
        sensor->configuration.sampling_rate_ms = sensor_config.configuration.sampling_rate_ms;

        sensor->configuration.connection_string = std::string(sensor_config.configuration.connection_string);
        sensor->configuration.UnwrapConnectionString();

        sensor->configuration.script_path = std::string(sensor_config.configuration.script_path);
        if(sd_fs_service_ != nullptr
            && sd_fs_service_->IsAvailable()
            && !sensor->configuration.script_path.empty()
            && sd_fs_service_->Exists(sensor->configuration.script_path)) {

            size_t script_size = sd_fs_service_->GetFileSize(sensor->configuration.script_path);

            if(script_size != 0) {
                std::pmr::vector<uint8_t> buffer(script_size, Mrm::GetExtPmr());

                size_t out_len = 0;
                sd_fs_service_->ReadFile(sensor->configuration.script_path, buffer.data(), script_size, out_len);

                sensor->configuration.lua_script = make_shared_pmr<LuaScript>(mr, LuaScript::CreateExt());
                sensor->configuration.lua_script->Load(std::span<const uint8_t>(buffer.data(), buffer.size()));
            }
        }

        InterpolationMethod interpolation_method = InterpolationMethod::NONE;
        if(sensor->configuration.type == SensorType::PHYSICAL_ANALOG)
            interpolation_method = GetInterpolationMethod(std::string(sensor_config.configuration.interpolation_method));

        if(interpolation_method != InterpolationMethod::NONE && sensor_config.configuration.calibration_table.size() > 0) {
            std::pmr::vector<CalibrationData> calibration_table(mr);
            for(auto& calibration_data : sensor_config.configuration.calibration_table) {
                calibration_table.push_back({
                    .voltage = calibration_data.voltage,
                    .value = calibration_data.value});
            }

            auto calibration_table_ptr = make_shared_pmr<std::pmr::vector<CalibrationData>>(mr, calibration_table);

            switch (interpolation_method) {
            case InterpolationMethod::LINEAR:
                sensor->configuration.voltage_interpolator = make_unique_pmr<LinearVoltageInterpolator>(mr, calibration_table_ptr);
                break;

            case InterpolationMethod::CUBIC_SPLINE:
                sensor->configuration.voltage_interpolator = make_unique_pmr<CubicSplineVoltageInterpolator>(mr, calibration_table_ptr);
                break;

            default:
                throw std::runtime_error("Sensor uses unsupported interpolation method.");
                break;
            }
        } else {
            sensor->configuration.voltage_interpolator = nullptr;
        }

        if(!sensor_config.configuration.expression.empty()) {
            sensor->configuration.expression_evaluator = make_unique_pmr<ExpressionEvaluator>(
                mr, std::string(sensor_config.configuration.expression));
        } else {
            sensor->configuration.expression_evaluator = nullptr;
        }

        order_resolver.AddSensor(std::move(sensor));
    }

    auto sensors = order_resolver.GetProcessingOrder();
    SensorValidator::Validate(sensors, sd_fs_service_.get(), gpio_channel_count, adc_channel_count);

    return sensors;
}

} // eerie_leap::domain::sensor_domain::configuration::parsers
