#include <zephyr/logging/log.h>

#include <utilities/memory/heap_allocator.hpp>
#include "sensors_configuration_controller.h"
#include "utilities/cbor/cbor_helpers.hpp"
#include "domain/sensor_domain/utilities/sensors_order_resolver.h"
#include "domain/sensor_domain/utilities/voltage_interpolator/linear_voltage_interpolator.hpp"
#include "domain/sensor_domain/utilities/voltage_interpolator/cubic_spline_voltage_interpolator.hpp"

namespace eerie_leap::controllers {

LOG_MODULE_REGISTER(sensors_config_ctrl_logger);

using namespace eerie_leap::utilities::memory;
using namespace eerie_leap::utilities::cbor;
using namespace eerie_leap::domain::sensor_domain::utilities;
using namespace eerie_leap::domain::sensor_domain::utilities::voltage_interpolator;

void SensorsConfigurationController::Initialize(std::shared_ptr<MathParserService> math_parser_service) {
    LOG_INF("Sensors Configuration Controller initialization started.");

    math_parser_service_ = math_parser_service;

    LOG_INF("Sensors Configuration Controller initialized successfully.");
}

bool SensorsConfigurationController::Update(const std::shared_ptr<std::vector<std::shared_ptr<Sensor>>> sensors) {
    auto sensors_config = make_shared_ext<SensorsConfig>();
    memset(sensors_config.get(), 0, sizeof(SensorsConfig));

    SensorsOrderResolver resolver;

    for(size_t i = 0; i < sensors->size(); ++i) {
        const auto& sensor = sensors->at(i);

        auto sensor_config = make_shared_ext<SensorConfig>();
        memset(sensor_config.get(), 0, sizeof(SensorConfig));

        sensor_config->id = CborHelpers::ToZcborString(&sensor->id);
        sensor_config->configuration.type = static_cast<uint32_t>(sensor->configuration.type);

        if(sensor->configuration.channel.has_value()) {
            sensor_config->configuration.channel_present = true;
            sensor_config->configuration.channel = sensor->configuration.channel.value();
        } else {
            sensor_config->configuration.channel_present = false;
        }

        sensor_config->configuration.sampling_rate_ms = sensor->configuration.sampling_rate_ms;

        auto interpolation_method = sensor->configuration.voltage_interpolator != nullptr
            ? sensor->configuration.voltage_interpolator->GetInterpolationMethod()
            : InterpolationMethod::NONE;
        sensor_config->configuration.interpolation_method = static_cast<uint32_t>(interpolation_method);
        if(interpolation_method != InterpolationMethod::NONE) {
            sensor_config->configuration.calibration_table_present = true;

            auto& calibration_table = *sensor->configuration.voltage_interpolator->GetCalibrationTable();
            sensor_config->configuration.calibration_table.floatfloat_count = calibration_table.size();

            std::sort(
                calibration_table.begin(),
                calibration_table.end(),
                [](const CalibrationData& a, const CalibrationData& b) {
                    return a.voltage < b.voltage;
                });

            for(size_t j = 0; j < calibration_table.size(); ++j) {
                const auto& calibration_data = calibration_table[j];
                sensor_config->configuration.calibration_table.floatfloat[j].floatfloat_key = calibration_data.voltage;
                sensor_config->configuration.calibration_table.floatfloat[j].floatfloat = calibration_data.value;
            }
        } else {
            sensor_config->configuration.calibration_table_present = false;
        }

        if(sensor->configuration.expression_evaluator != nullptr) {
            sensor_config->configuration.expression_present = true;
            sensor_config->configuration.expression = CborHelpers::ToZcborString(sensor->configuration.expression_evaluator->GetRawExpression());
        } else {
            sensor_config->configuration.expression_present = false;
        }

        sensor_config->metadata.unit = CborHelpers::ToZcborString(&sensor->metadata.unit);
        sensor_config->metadata.name = CborHelpers::ToZcborString(&sensor->metadata.name);

        if(sensor->metadata.description.has_value()) {
            sensor_config->metadata.description_present = true;
            sensor_config->metadata.description = CborHelpers::ToZcborString(&sensor->metadata.description.value());
        } else {
            sensor_config->metadata.description_present = false;
        }

        sensors_config->SensorConfig_m[i] = *sensor_config.get();
        sensors_config->SensorConfig_m_count++;

        resolver.AddSensor(sensor);
    }

    sensors_config_ = sensors_config;
    auto ordered_sensors = resolver.GetProcessingOrder();
    ordered_sensors_ = std::make_shared<std::vector<std::shared_ptr<Sensor>>>(ordered_sensors);

    LOG_INF("Going to save.");
    return sensors_configuration_service_->Save(sensors_config.get());
}

const std::shared_ptr<std::vector<std::shared_ptr<Sensor>>> SensorsConfigurationController::Get() {
    if(ordered_sensors_ != nullptr)
        return ordered_sensors_;

    auto sensors_config = sensors_configuration_service_->Load();
    if(!sensors_config.has_value())
        return nullptr;

    sensors_config_ = make_shared_ext<SensorsConfig>(sensors_config.value());
    SensorsOrderResolver resolver;

    for(size_t i = 0; i < sensors_config_->SensorConfig_m_count; ++i) {
        const auto& sensor_config = sensors_config_->SensorConfig_m[i];
        std::shared_ptr<Sensor> sensor = std::make_shared<Sensor>();

        sensor->id = std::string(reinterpret_cast<const char*>(sensor_config.id.value), sensor_config.id.len);
        sensor->configuration.type = static_cast<SensorType>(sensor_config.configuration.type);

        if(sensor_config.configuration.channel_present)
            sensor->configuration.channel = sensor_config.configuration.channel;
        else
            sensor->configuration.channel = std::nullopt;

        sensor->configuration.sampling_rate_ms = sensor_config.configuration.sampling_rate_ms;

        auto interpolation_method = static_cast<InterpolationMethod>(sensor_config.configuration.interpolation_method);
        if(interpolation_method != InterpolationMethod::NONE && sensor_config.configuration.calibration_table_present) {
            std::vector<CalibrationData> calibration_table;
            for(size_t j = 0; j < sensor_config.configuration.calibration_table.floatfloat_count; ++j) {
                const auto& calibration_data = sensor_config.configuration.calibration_table.floatfloat[j];

                calibration_table.push_back({
                    .voltage = calibration_data.floatfloat_key,
                    .value = calibration_data.floatfloat});
            }

            auto calibration_table_ptr = std::make_shared<std::vector<CalibrationData>>(calibration_table);

            switch (interpolation_method)
            {
            case InterpolationMethod::LINEAR:
                sensor->configuration.voltage_interpolator = std::make_shared<LinearVoltageInterpolator>(calibration_table_ptr);
                break;

            case InterpolationMethod::CUBIC_SPLINE:
                sensor->configuration.voltage_interpolator = std::make_shared<CubicSplineVoltageInterpolator>(calibration_table_ptr);
                break;

            default:
                throw std::runtime_error("Sensor uses unsupported interpolation method!");
                break;
            }
        } else {
            sensor->configuration.voltage_interpolator = nullptr;
        }

        if(sensor_config.configuration.expression_present)
            sensor->configuration.expression_evaluator = std::make_shared<ExpressionEvaluator>(
                math_parser_service_,
                CborHelpers::ToStdString(sensor_config.configuration.expression));
        else
            sensor->configuration.expression_evaluator = nullptr;

        sensor->metadata.unit = CborHelpers::ToStdString(sensor_config.metadata.unit);
        sensor->metadata.name = CborHelpers::ToStdString(sensor_config.metadata.name);

        sensor->metadata.description = sensor_config.metadata.description_present
            ? CborHelpers::ToStdString(sensor_config.metadata.description)
            : "";

        resolver.AddSensor(sensor);
    }

    ordered_sensors_ = std::make_shared<std::vector<std::shared_ptr<Sensor>>>(resolver.GetProcessingOrder());

    return ordered_sensors_;
}

} // namespace eerie_leap::domain::sensor_domain::controllers
