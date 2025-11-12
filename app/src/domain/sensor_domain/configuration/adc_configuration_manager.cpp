#include <memory>
#include <vector>

#include "utilities/memory/heap_allocator.h"
#include "utilities/voltage_interpolator/calibration_data.h"
#include "utilities/voltage_interpolator/interpolation_method.h"
#include "subsys/adc/adc_factory.hpp"

#include "adc_configuration_manager.h"

namespace eerie_leap::domain::sensor_domain::configuration {

using namespace eerie_leap::utilities::memory;
using namespace eerie_leap::utilities::voltage_interpolator;
using namespace eerie_leap::subsys::adc;

LOG_MODULE_REGISTER(adc_config_ctrl_logger);

AdcConfigurationManager::AdcConfigurationManager(ext_unique_ptr<CborConfigurationService<CborAdcConfig>> cbor_configuration_service) :
    cbor_configuration_service_(std::move(cbor_configuration_service)),
    adc_manager_(AdcFactory::Create()),
    config_(nullptr),
    configuration_(nullptr) {

    cbor_parser_ = std::make_unique<AdcConfigurationCborParser>();
    adc_manager_->Initialize();

    std::shared_ptr<IAdcManager> adc_manager = nullptr;

    try {
        adc_manager = Get(true);
    } catch(const std::exception& e) {}

    if(adc_manager == nullptr) {
        LOG_ERR("Failed to load ADC configuration.");

        if(!CreateDefaultConfiguration()) {
            LOG_ERR("Failed to create default ADC configuration.");
            return;
        }

        LOG_INF("Default ADC configuration loaded successfully.");
    } else {
        LOG_INF("ADC Configuration Manager initialized successfully.");
    }
}

bool AdcConfigurationManager::Update(const AdcConfiguration& configuration) {
    auto config = cbor_parser_->Serialize(configuration);

    LOG_INF("Saving ADCs configuration.");
    if(!cbor_configuration_service_->Save(config.get()))
        return false;

    Get(true);

    return true;
}

std::shared_ptr<IAdcManager> AdcConfigurationManager::Get(bool force_load) {
    if (configuration_ != nullptr && !force_load) {
        return adc_manager_;
    }

    auto config = cbor_configuration_service_->Load();
    if(!config.has_value())
        return nullptr;

    config_raw_ = std::move(config.value().config_raw);
    config_ = std::move(config.value().config);

    auto configuration = cbor_parser_->Deserialize(*config_);
    configuration_ = make_shared_ext<AdcConfiguration>(configuration);
    adc_manager_->UpdateConfiguration(configuration_);

    return adc_manager_;
}

// TODO: Refine default configuration
bool AdcConfigurationManager::CreateDefaultConfiguration() {
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

    auto configuration = make_shared_ext<AdcConfiguration>();
    configuration->samples = 40;
    configuration->channel_configurations =
        make_shared_ext<std::vector<std::shared_ptr<AdcChannelConfiguration>>>(channel_configurations);

    return Update(*configuration.get());
}

} // namespace eerie_leap::domain::sensor_domain::configuration
