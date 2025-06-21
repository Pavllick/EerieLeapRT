#include <ranges>
#include <memory>

#include "utilities/memory/heap_allocator.h"
#include "utilities/cbor/cbor_helpers.hpp"
#include "utilities/voltage_interpolator/linear_voltage_interpolator.hpp"
#include "utilities/voltage_interpolator/linear_voltage_interpolator.hpp"
#include "utilities/voltage_interpolator/cubic_spline_voltage_interpolator.hpp"
#include "adc_configuration_controller.h"

namespace eerie_leap::controllers {

using namespace eerie_leap::utilities::memory;
using namespace eerie_leap::utilities::voltage_interpolator;

LOG_MODULE_REGISTER(adc_config_ctrl_logger);

AdcConfigurationController::AdcConfigurationController(std::shared_ptr<ConfigurationService<AdcConfig>> adc_configuration_service, std::shared_ptr<IAdcManager> adc_manager) :
    adc_configuration_service_(std::move(adc_configuration_service)),
    adc_manager_(std::move(adc_manager)),
    adc_config_(nullptr),
    adc_configuration_(nullptr) {

    if(Get(true) == nullptr)
        LOG_ERR("Failed to load ADC configuration.");
    else
        LOG_INF("ADC Configuration Controller initialized successfully.");
}

bool AdcConfigurationController::Update(const std::shared_ptr<AdcConfiguration>& adc_configuration) {
    auto adc_config = make_shared_ext<AdcConfig>();
    memset(adc_config.get(), 0, sizeof(AdcConfig));

    adc_config->samples = adc_configuration->samples;

    for(size_t i = 0; i < adc_configuration->channel_configurations->size(); ++i) {
        auto adc_channel_config = make_shared_ext<AdcChannelConfig>();
        memset(adc_channel_config.get(), 0, sizeof(AdcChannelConfig));

        auto interpolation_method = adc_configuration->channel_configurations->at(i)->voltage_interpolator != nullptr
            ? adc_configuration->channel_configurations->at(i)->voltage_interpolator->GetInterpolationMethod()
            : InterpolationMethod::NONE;

        adc_channel_config->interpolation_method = static_cast<uint32_t>(interpolation_method);
        if(interpolation_method != InterpolationMethod::NONE) {
            adc_channel_config->calibration_table_present = true;

            auto& calibration_table = *adc_configuration->channel_configurations->at(i)->voltage_interpolator->GetCalibrationTable();
            adc_channel_config->calibration_table.float32float_count = calibration_table.size();

            if(calibration_table.size() < 2)
                throw std::runtime_error("Calibration table must have at least 2 points!");

            std::ranges::sort(
                calibration_table,
                [](const CalibrationData& a, const CalibrationData& b) {
                    return a.voltage < b.voltage;
                });

            for(size_t j = 0; j < calibration_table.size(); ++j) {
                const auto& calibration_data = calibration_table[j];
                adc_channel_config->calibration_table.float32float[j].float32float_key = calibration_data.voltage;
                adc_channel_config->calibration_table.float32float[j].float32float = calibration_data.value;
            }
        } else {
            adc_channel_config->calibration_table_present = false;
        }

        adc_config->AdcChannelConfig_m[i] = *adc_channel_config;
        adc_config->AdcChannelConfig_m_count++;
    }

    LOG_INF("Saving ADCs configuration.");
    if(!adc_configuration_service_->Save(adc_config.get()))
        return false;

    Get(true);

    return true;
}

std::shared_ptr<IAdcManager> AdcConfigurationController::Get(bool force_load) {
    if (adc_configuration_ != nullptr && !force_load) {
        return adc_manager_;
    }

    auto adc_configuration = std::make_shared<AdcConfiguration>();

    auto adc_config = adc_configuration_service_->Load();
    if(!adc_config.has_value())
        return nullptr;

    adc_config_raw_ = adc_config.value().config_raw;
    adc_config_ = adc_config.value().config;

    adc_configuration->samples = static_cast<uint16_t>(adc_config_->samples);
    adc_configuration->channel_configurations = make_shared_ext<std::vector<std::shared_ptr<AdcChannelConfiguration>>>();

    for(size_t i = 0; i < adc_config_->AdcChannelConfig_m_count; ++i) {
        const auto& adc_channel_config = adc_config_->AdcChannelConfig_m[i];
        auto adc_channel_configuration = make_shared_ext<AdcChannelConfiguration>();

        auto interpolation_method = static_cast<InterpolationMethod>(adc_channel_config.interpolation_method);
        if(interpolation_method != InterpolationMethod::NONE && adc_channel_config.calibration_table_present) {
            std::vector<CalibrationData> calibration_table;
            for(size_t j = 0; j < adc_channel_config.calibration_table.float32float_count; ++j) {
                const auto& calibration_data = adc_channel_config.calibration_table.float32float[j];

                calibration_table.push_back({
                        .voltage = calibration_data.float32float_key,
                        .value = calibration_data.float32float});
                }

                auto calibration_table_ptr = make_shared_ext<std::vector<CalibrationData>>(calibration_table);

                switch (interpolation_method) {
                case InterpolationMethod::LINEAR:
                    adc_channel_configuration->voltage_interpolator = make_shared_ext<LinearVoltageInterpolator>(calibration_table_ptr);
                    break;

                case InterpolationMethod::CUBIC_SPLINE:
                    adc_channel_configuration->voltage_interpolator = make_shared_ext<CubicSplineVoltageInterpolator>(calibration_table_ptr);
                    break;

                default:
                    throw std::runtime_error("Sensor uses unsupported interpolation method!");
                    break;
                }
            } else {
                adc_channel_configuration->voltage_interpolator = nullptr;
            }

        adc_configuration->channel_configurations->push_back(adc_channel_configuration);
    }

    adc_configuration_ = adc_configuration;
    adc_manager_->UpdateConfiguration(adc_configuration_);

    return adc_manager_;
}

} // namespace eerie_leap::controllers
