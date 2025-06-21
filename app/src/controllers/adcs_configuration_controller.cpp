#include <ranges>
#include <memory>

#include "utilities/memory/heap_allocator.h"
#include "utilities/cbor/cbor_helpers.hpp"
#include "utilities/voltage_interpolator/linear_voltage_interpolator.hpp"
#include "utilities/voltage_interpolator/linear_voltage_interpolator.hpp"
#include "utilities/voltage_interpolator/cubic_spline_voltage_interpolator.hpp"
#include "adcs_configuration_controller.h"

namespace eerie_leap::controllers {

using namespace eerie_leap::utilities::memory;
using namespace eerie_leap::utilities::voltage_interpolator;

LOG_MODULE_REGISTER(adc_config_ctrl_logger);

AdcsConfigurationController::AdcsConfigurationController(std::shared_ptr<ConfigurationService<AdcsConfig>> adcs_configuration_service, std::shared_ptr<IAdcManager> adc_manager) :
    adcs_configuration_service_(std::move(adcs_configuration_service)),
    adc_manager_(std::move(adc_manager)),
    adcs_config_(nullptr),
    adcs_configuration_(nullptr) {

    // if(Get(true) == nullptr)
    //     LOG_ERR("Failed to load ADC configuration.");
    // else
    //     LOG_INF("ADC Configuration Controller initialized successfully.");
}

bool AdcsConfigurationController::Update(const std::shared_ptr<std::vector<std::shared_ptr<AdcConfiguration>>>& adcs_configuration) {
    auto adcs_config = make_shared_ext<AdcsConfig>();
    memset(adcs_config.get(), 0, sizeof(AdcsConfig));

    for(size_t i = 0; i < adcs_configuration->size(); ++i) {
        auto adc_config = make_shared_ext<AdcConfig>();
        memset(adc_config.get(), 0, sizeof(AdcConfig));

        adc_config->name = CborHelpers::ToZcborString(&adcs_configuration->at(i)->name);
        adc_config->samples = adcs_configuration->at(i)->samples;

        auto interpolation_method = adcs_configuration->at(i)->voltage_interpolator != nullptr
            ? adcs_configuration->at(i)->voltage_interpolator->GetInterpolationMethod()
            : InterpolationMethod::NONE;

        adc_config->interpolation_method = static_cast<uint32_t>(interpolation_method);
        if(interpolation_method != InterpolationMethod::NONE) {
            adc_config->calibration_table_present = true;

            auto& calibration_table = *adcs_configuration->at(i)->voltage_interpolator->GetCalibrationTable();
            adc_config->calibration_table.float32float_count = calibration_table.size();

            if(calibration_table.size() < 2)
                throw std::runtime_error("Calibration table must have at least 2 points!");

            std::ranges::sort(
                calibration_table,
                [](const CalibrationData& a, const CalibrationData& b) {
                    return a.voltage < b.voltage;
                });

            for(size_t j = 0; j < calibration_table.size(); ++j) {
                const auto& calibration_data = calibration_table[j];
                adc_config->calibration_table.float32float[j].float32float_key = calibration_data.voltage;
                adc_config->calibration_table.float32float[j].float32float = calibration_data.value;
            }
        } else {
            adc_config->calibration_table_present = false;
        }

        adcs_config->AdcConfig_m[i] = *adc_config;
        adcs_config->AdcConfig_m_count++;
    }

    LOG_INF("Saving ADCs configuration.");
    if(!adcs_configuration_service_->Save(adcs_config.get()))
        return false;

    Get(true);

    return true;
}

std::shared_ptr<IAdcManager> AdcsConfigurationController::Get(bool force_load) {
    if (adcs_configuration_ != nullptr && !force_load) {
        return adc_manager_;
    }

    std::shared_ptr<std::vector<std::shared_ptr<AdcConfiguration>>> adcs_configuration = std::make_shared<std::vector<std::shared_ptr<AdcConfiguration>>>();

    auto adcs_config = adcs_configuration_service_->Load();
    if(!adcs_config.has_value())
        return nullptr;

    adcs_config_raw_ = adcs_config.value().config_raw;
    adcs_config_ = adcs_config.value().config;

    for(size_t i = 0; i < adcs_config_->AdcConfig_m_count; ++i) {
        const auto& adc_config = adcs_config_->AdcConfig_m[i];
        auto adc_configuration = make_shared_ext<AdcConfiguration>();

        adc_configuration->name = CborHelpers::ToStdString(adc_config.name);
        adc_configuration->samples = static_cast<uint16_t>(adc_config.samples);

        auto interpolation_method = static_cast<InterpolationMethod>(adc_config.interpolation_method);
        if(interpolation_method != InterpolationMethod::NONE && adc_config.calibration_table_present) {
            std::vector<CalibrationData> calibration_table;
            for(size_t j = 0; j < adc_config.calibration_table.float32float_count; ++j) {
                const auto& calibration_data = adc_config.calibration_table.float32float[j];

                calibration_table.push_back({
                        .voltage = calibration_data.float32float_key,
                        .value = calibration_data.float32float});
                }

                auto calibration_table_ptr = make_shared_ext<std::vector<CalibrationData>>(calibration_table);

                switch (interpolation_method) {
                case InterpolationMethod::LINEAR:
                    adc_configuration->voltage_interpolator = make_shared_ext<LinearVoltageInterpolator>(calibration_table_ptr);
                    break;

                case InterpolationMethod::CUBIC_SPLINE:
                    adc_configuration->voltage_interpolator = make_shared_ext<CubicSplineVoltageInterpolator>(calibration_table_ptr);
                    break;

                default:
                    throw std::runtime_error("Sensor uses unsupported interpolation method!");
                    break;
                }
            } else {
                adc_configuration->voltage_interpolator = nullptr;
            }

        adcs_configuration->push_back(adc_configuration);
    }

    adcs_configuration_ = adcs_configuration;
    adc_manager_->UpdateConfigurations(adcs_configuration_);

    return adc_manager_;
}

} // namespace eerie_leap::controllers
