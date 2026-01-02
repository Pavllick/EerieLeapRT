#include <utility>

#include <zephyr/kernel.h>

#include "utilities/cbor/cbor_helpers.hpp"
#include "utilities/voltage_interpolator/linear_voltage_interpolator.hpp"
#include "utilities/voltage_interpolator/cubic_spline_voltage_interpolator.hpp"
#include "subsys/lua_script/lua_script.h"
#include "domain/sensor_domain/utilities/sensors_order_resolver.h"
#include "sensor_validator.h"

#include "sensors_cbor_parser.h"

namespace eerie_leap::domain::sensor_domain::configuration::parsers {

using namespace eerie_leap::utilities::cbor;
using namespace eerie_leap::utilities::voltage_interpolator;
using namespace eerie_leap::subsys::lua_script;
using namespace eerie_leap::domain::sensor_domain::utilities;

SensorsCborParser::SensorsCborParser(std::shared_ptr<IFsService> sd_fs_service)
    : sd_fs_service_(std::move(sd_fs_service)) {}

pmr_unique_ptr<CborSensorsConfig> SensorsCborParser::Serialize(
    const std::vector<std::shared_ptr<Sensor>>& sensors,
    uint32_t gpio_channel_count,
    uint32_t adc_channel_count) {

    // NOTE: UpdateConnectionString() must be called before validation
    for(auto& sensor : sensors)
        sensor->configuration.UpdateConnectionString();
    SensorValidator::Validate(sensors, sd_fs_service_.get(), gpio_channel_count, adc_channel_count);

    auto sensors_config = make_unique_pmr<CborSensorsConfig>(Mrm::GetExtPmr());
    SensorsOrderResolver order_resolver;

    for(const auto& sensor : sensors) {

        CborSensorConfig sensor_config(std::allocator_arg, Mrm::GetExtPmr());

        sensor_config.id = CborHelpers::ToZcborString(sensor->id);
        sensor_config.configuration.type = std::to_underlying(sensor->configuration.type);

        if(sensor->configuration.channel.has_value()) {
            sensor_config.configuration.channel_present = true;
            sensor_config.configuration.channel = sensor->configuration.channel.value();
        } else {
            sensor_config.configuration.channel_present = false;
        }

        sensor_config.configuration.connection_string = CborHelpers::ToZcborString(sensor->configuration.connection_string);
        sensor_config.configuration.script_path = CborHelpers::ToZcborString(sensor->configuration.script_path);
        sensor_config.configuration.sampling_rate_ms = sensor->configuration.sampling_rate_ms;

        auto interpolation_method = sensor->configuration.voltage_interpolator != nullptr
            ? sensor->configuration.voltage_interpolator->GetInterpolationMethod()
            : InterpolationMethod::NONE;

        sensor_config.configuration.interpolation_method = static_cast<uint32_t>(interpolation_method);
        if(interpolation_method != InterpolationMethod::NONE) {
            sensor_config.configuration.calibration_table_present = true;

            auto& calibration_table = *sensor->configuration.voltage_interpolator->GetCalibrationTable();

            std::ranges::sort(
                calibration_table,
                [](const CalibrationData& a, const CalibrationData& b) {
                    return a.voltage < b.voltage;
                });

            for(const auto& calibration_data : calibration_table) {
                sensor_config.configuration.calibration_table.float32float.push_back({
                    .float32float_key = calibration_data.voltage,
                    .float32float = calibration_data.value});
            }
        } else {
            sensor_config.configuration.calibration_table_present = false;
        }

        if(sensor->configuration.expression_evaluator != nullptr) {
            sensor_config.configuration.expression_present = true;
            sensor_config.configuration.expression = CborHelpers::ToZcborString(sensor->configuration.expression_evaluator->GetExpression());
        } else {
            sensor_config.configuration.expression_present = false;
        }

        sensor_config.metadata.unit = CborHelpers::ToZcborString(sensor->metadata.unit);
        sensor_config.metadata.name = CborHelpers::ToZcborString(sensor->metadata.name);

        sensor_config.metadata.description = CborHelpers::ToZcborString(sensor->metadata.description);

        sensors_config->CborSensorConfig_m.push_back(std::move(sensor_config));

        order_resolver.AddSensor(sensor);
    }

    // Validate dependencies
    order_resolver.GetProcessingOrder();

    return sensors_config;
}

std::vector<std::shared_ptr<Sensor>> SensorsCborParser::Deserialize(
    std::pmr::memory_resource* mr,
    const CborSensorsConfig& sensors_config,
    uint32_t gpio_channel_count,
    uint32_t adc_channel_count) {

    SensorsOrderResolver order_resolver;

    for(const auto& sensor_config : sensors_config.CborSensorConfig_m) {
        auto sensor = make_shared_pmr<Sensor>(mr, CborHelpers::ToPmrString(mr, sensor_config.id));

        sensor->configuration.type = static_cast<SensorType>(sensor_config.configuration.type);

        if(sensor_config.configuration.channel_present)
            sensor->configuration.channel = sensor_config.configuration.channel;
        else
            sensor->configuration.channel = std::nullopt;

        sensor->configuration.connection_string = CborHelpers::ToPmrString(mr, sensor_config.configuration.connection_string);
        sensor->configuration.UnwrapConnectionString();

        sensor->configuration.script_path = CborHelpers::ToPmrString(mr, sensor_config.configuration.script_path);
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

        sensor->configuration.sampling_rate_ms = sensor_config.configuration.sampling_rate_ms;

        auto interpolation_method = static_cast<InterpolationMethod>(sensor_config.configuration.interpolation_method);
        if(interpolation_method != InterpolationMethod::NONE && sensor_config.configuration.calibration_table_present) {
            std::pmr::vector<CalibrationData> calibration_table(mr);
            calibration_table.reserve(sensor_config.configuration.calibration_table.float32float.size());
            for(const auto& calibration_data : sensor_config.configuration.calibration_table.float32float) {
                calibration_table.push_back({
                    .voltage = calibration_data.float32float_key,
                    .value = calibration_data.float32float});
            }

            auto calibration_table_ptr = make_shared_pmr<std::pmr::vector<CalibrationData>>(mr, calibration_table);

            switch(interpolation_method) {
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

        if(sensor_config.configuration.expression_present)
            sensor->configuration.expression_evaluator = make_unique_pmr<ExpressionEvaluator>(
                mr, CborHelpers::ToStdString(sensor_config.configuration.expression));
        else
            sensor->configuration.expression_evaluator = nullptr;

        sensor->metadata.unit = CborHelpers::ToPmrString(mr, sensor_config.metadata.unit);
        sensor->metadata.name = CborHelpers::ToPmrString(mr, sensor_config.metadata.name);

        sensor->metadata.description = CborHelpers::ToPmrString(mr, sensor_config.metadata.description);

        order_resolver.AddSensor(std::move(sensor));
    }

    auto sensors = order_resolver.GetProcessingOrder();
    SensorValidator::Validate(sensors, sd_fs_service_.get(), gpio_channel_count, adc_channel_count);

    return sensors;
}

} // namespace eerie_leap::domain::sensor_domain::configuration::parsers
