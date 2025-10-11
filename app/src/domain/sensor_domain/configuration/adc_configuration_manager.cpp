#include <ranges>
#include <memory>
#include <vector>

#include "utilities/memory/heap_allocator.h"
#include "utilities/cbor/cbor_helpers.hpp"
#include "utilities/voltage_interpolator/calibration_data.h"
#include "utilities/voltage_interpolator/linear_voltage_interpolator.hpp"
#include "utilities/voltage_interpolator/linear_voltage_interpolator.hpp"
#include "utilities/voltage_interpolator/cubic_spline_voltage_interpolator.hpp"
#include "subsys/adc/adc_factory.hpp"
#include "adc_configuration_manager.h"

namespace eerie_leap::domain::sensor_domain::configuration {

using namespace eerie_leap::utilities::memory;
using namespace eerie_leap::utilities::voltage_interpolator;
using namespace eerie_leap::subsys::adc;

LOG_MODULE_REGISTER(adc_config_ctrl_logger);

AdcConfigurationManager::AdcConfigurationManager(std::shared_ptr<ConfigurationService<AdcConfig>> adc_configuration_service) :
    adc_configuration_service_(std::move(adc_configuration_service)),
    adc_manager_(AdcFactory::Create()),
    adc_config_(nullptr),
    adc_configuration_(nullptr) {

    adc_manager_->Initialize();

    if(Get(true) == nullptr) {
        LOG_ERR("Failed to load ADC configuration.");

        SetDefaultConfiguration();

        LOG_INF("Default ADC configuration loaded successfully.");
    } else {
        LOG_INF("ADC Configuration Controller initialized successfully.");
    }
}

bool AdcConfigurationManager::Update(const std::shared_ptr<AdcConfiguration>& adc_configuration) {
    auto adc_config = make_shared_ext<AdcConfig>();
    memset(adc_config.get(), 0, sizeof(AdcConfig));

    adc_config->samples = adc_configuration->samples;

    for(size_t i = 0; i < adc_configuration->channel_configurations->size(); ++i) {
        auto adc_channel_config = make_shared_ext<AdcChannelConfig>();
        memset(adc_channel_config.get(), 0, sizeof(AdcChannelConfig));

        auto interpolation_method = adc_configuration->channel_configurations->at(i)->calibrator != nullptr
            ? adc_configuration->channel_configurations->at(i)->calibrator->GetInterpolationMethod()
            : InterpolationMethod::NONE;

        adc_channel_config->interpolation_method = static_cast<uint32_t>(interpolation_method);
        if(interpolation_method != InterpolationMethod::NONE) {
            adc_channel_config->calibration_table_present = true;

            auto& calibration_table = *adc_configuration->channel_configurations->at(i)->calibrator->GetCalibrationTable();
            adc_channel_config->calibration_table.float32float_count = calibration_table.size();

            if(calibration_table.size() < 2)
                throw std::runtime_error("Calibration table must have at least 2 points.");

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
            throw std::runtime_error("ADC channel configuration is invalid. Calibration table is missing.");
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

std::shared_ptr<IAdcManager> AdcConfigurationManager::Get(bool force_load) {
    if (adc_configuration_ != nullptr && !force_load) {
        return adc_manager_;
    }

    auto adc_configuration = make_shared_ext<AdcConfiguration>();

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
                adc_channel_configuration->calibrator = make_shared_ext<AdcCalibrator>(interpolation_method, calibration_table_ptr);
            } else {
                throw std::runtime_error("ADC channel configuration is invalid. Calibration table is missing.");
            }

        adc_configuration->channel_configurations->push_back(adc_channel_configuration);
    }

    adc_configuration_ = adc_configuration;
    adc_manager_->UpdateConfiguration(adc_configuration_);

    return adc_manager_;
}

void AdcConfigurationManager::SetDefaultConfiguration() {
    std::vector<CalibrationData> adc_calibration_data_samples {
        {0.501, 0.469},
        {1.0, 0.968},
        {2.0, 1.970},
        {3.002, 2.98},
        {4.003, 4.01},
        {5.0, 5.0}
    };

    auto adc_calibration_data_samples_ptr = make_shared_ext<std::vector<CalibrationData>>(adc_calibration_data_samples);
    auto adc_calibrator = make_shared_ext<AdcCalibrator>(InterpolationMethod::LINEAR, adc_calibration_data_samples_ptr);

    std::vector<std::shared_ptr<AdcChannelConfiguration>> channel_configurations;
    channel_configurations.reserve(adc_manager_->GetChannelCount());
    for(size_t i = 0; i < adc_manager_->GetChannelCount(); ++i)
        channel_configurations.push_back(make_shared_ext<AdcChannelConfiguration>(adc_calibrator));

    auto adc_configuration = make_shared_ext<AdcConfiguration>();
    adc_configuration->samples = 40;
    adc_configuration->channel_configurations =
        make_shared_ext<std::vector<std::shared_ptr<AdcChannelConfiguration>>>(channel_configurations);

    if(!Update(adc_configuration))
        throw std::runtime_error("Cannot save ADCs config");
}

} // namespace eerie_leap::domain::sensor_domain::configuration
