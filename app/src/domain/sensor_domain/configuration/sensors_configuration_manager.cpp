#include <functional>

#include <zephyr/logging/log.h>

#include "sensors_configuration_manager.h"
#include "utilities/cbor/cbor_helpers.hpp"
#include "domain/sensor_domain/utilities/sensors_order_resolver.h"
#include "utilities/voltage_interpolator/linear_voltage_interpolator.hpp"
#include "utilities/voltage_interpolator/cubic_spline_voltage_interpolator.hpp"

namespace eerie_leap::domain::sensor_domain::configuration {

LOG_MODULE_REGISTER(sensors_config_ctrl_logger);

using namespace eerie_leap::utilities::cbor;
using namespace eerie_leap::domain::sensor_domain::utilities;
using namespace eerie_leap::utilities::voltage_interpolator;

SensorsConfigurationManager::SensorsConfigurationManager(
    std::shared_ptr<MathParserService> math_parser_service,
    std::shared_ptr<ConfigurationService<SensorsConfig>> sensors_configuration_service,
    int adc_channel_count) :

    math_parser_service_(std::move(math_parser_service)),
    sensors_configuration_service_(std::move(sensors_configuration_service)),
    sensors_config_(make_shared_ext<SensorsConfig>()),
    ordered_sensors_(make_shared_ext<std::vector<std::shared_ptr<Sensor>>>()),
    adc_channel_count_(adc_channel_count) {

    if(Get(true) == nullptr)
        LOG_ERR("Failed to load sensors configuration.");
    else
        LOG_INF("Sensors Configuration Controller initialized successfully.");
}

bool SensorsConfigurationManager::Update(const std::shared_ptr<std::vector<std::shared_ptr<Sensor>>>& sensors) {
    auto sensors_config = make_shared_ext<SensorsConfig>();
    memset(sensors_config.get(), 0, sizeof(SensorsConfig));

    SensorsOrderResolver resolver;
    std::hash<std::string> string_hasher;

    for(size_t i = 0; i < sensors->size(); ++i) {
        const auto& sensor = sensors->at(i);

        auto sensor_config = make_shared_ext<SensorConfig>();
        memset(sensor_config.get(), 0, sizeof(SensorConfig));

        sensor_config->id = CborHelpers::ToZcborString(&sensor->id);
        sensor_config->id_hash = static_cast<uint32_t>(string_hasher(sensor->id));
        sensor_config->configuration.type = static_cast<uint32_t>(sensor->configuration.type);

        if(sensor->configuration.channel.has_value()) {
            if(sensor->configuration.channel.value() < 0 || sensor->configuration.channel.value() > adc_channel_count_)
                throw std::runtime_error("Invalid channel value.");

            sensor_config->configuration.channel_present = true;
            sensor_config->configuration.channel = sensor->configuration.channel.value();
        } else {
            sensor_config->configuration.channel_present = false;
        }

        sensor_config->configuration.sampling_rate_ms = sensor->configuration.sampling_rate_ms;

        auto interpolation_method = sensor->configuration.voltage_interpolator != nullptr
            ? sensor->configuration.voltage_interpolator->GetInterpolationMethod()
            : InterpolationMethod::NONE;
        if(sensor->configuration.type == SensorType::PHYSICAL_ANALOG && interpolation_method == InterpolationMethod::NONE)
            throw std::runtime_error("Physical analog sensor must have interpolation method.");

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
            sensor_config->configuration.expression = CborHelpers::ToZcborString(sensor->configuration.expression_evaluator->GetRawExpression());
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

    LOG_INF("Saving sensors configuration.");
    bool res = sensors_configuration_service_->Save(sensors_config.get());
    if(!res)
        return false;

    Get(true);

    return true;
}

const std::shared_ptr<std::vector<std::shared_ptr<Sensor>>> SensorsConfigurationManager::Get(bool force_load) {
    if(ordered_sensors_ != nullptr && !force_load)
        return ordered_sensors_;

    auto sensors_config = sensors_configuration_service_->Load();
    if(!sensors_config.has_value())
        return nullptr;

    sensors_config_raw_ = sensors_config.value().config_raw;
    sensors_config_ = sensors_config.value().config;
    SensorsOrderResolver resolver;

    for(size_t i = 0; i < sensors_config_->SensorConfig_m_count; ++i) {
        const auto& sensor_config = sensors_config_->SensorConfig_m[i];
        std::shared_ptr<Sensor> sensor = make_shared_ext<Sensor>();

        sensor->id = CborHelpers::ToStdString(sensor_config.id);
        sensor->id_hash = sensor_config.id_hash;
        sensor->configuration.type = static_cast<SensorType>(sensor_config.configuration.type);

        if(sensor_config.configuration.channel_present)
            sensor->configuration.channel = sensor_config.configuration.channel;
        else
            sensor->configuration.channel = std::nullopt;

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
                sensor->configuration.voltage_interpolator = make_shared_ext<LinearVoltageInterpolator>(calibration_table_ptr);
                break;

            case InterpolationMethod::CUBIC_SPLINE:
                sensor->configuration.voltage_interpolator = make_shared_ext<CubicSplineVoltageInterpolator>(calibration_table_ptr);
                break;

            default:
                throw std::runtime_error("Sensor uses unsupported interpolation method.");
                break;
            }
        } else {
            sensor->configuration.voltage_interpolator = nullptr;
        }

        if(sensor_config.configuration.expression_present)
            sensor->configuration.expression_evaluator = make_shared_ext<ExpressionEvaluator>(
                math_parser_service_,
                CborHelpers::ToStdString(sensor_config.configuration.expression));
        else
            sensor->configuration.expression_evaluator = nullptr;

        sensor->metadata.unit = CborHelpers::ToStdString(sensor_config.metadata.unit);
        sensor->metadata.name = CborHelpers::ToStdString(sensor_config.metadata.name);

        sensor->metadata.description = CborHelpers::ToStdString(sensor_config.metadata.description);

        resolver.AddSensor(sensor);
    }

    ordered_sensors_ = make_shared_ext<std::vector<std::shared_ptr<Sensor>>>(resolver.GetProcessingOrder());

    return ordered_sensors_;
}

const std::span<uint8_t> SensorsConfigurationManager::GetRaw() {
    return *sensors_config_raw_.get();
}

} // namespace eerie_leap::domain::sensor_domain::configuration
