#include <memory>
#include <vector>

#include "utilities/voltage_interpolator/calibration_data.h"
#include "utilities/voltage_interpolator/interpolation_method.h"
#include "subsys/adc/adc_factory.hpp"

#include "adc_configuration_manager.h"

namespace eerie_leap::domain::sensor_domain::configuration {

using namespace eerie_leap::utilities::voltage_interpolator;
using namespace eerie_leap::subsys::adc;

LOG_MODULE_REGISTER(adc_config_ctrl_logger);

AdcConfigurationManager::AdcConfigurationManager(
    std::unique_ptr<CborConfigurationService<CborAdcConfig>> cbor_configuration_service,
    std::unique_ptr<JsonConfigurationService<JsonAdcConfig>> json_configuration_service)
        : cbor_configuration_service_(std::move(cbor_configuration_service)),
        json_configuration_service_(std::move(json_configuration_service)),
        adc_manager_(AdcFactory::Create()),
        configuration_(nullptr),
        json_config_checksum_(0) {

    cbor_parser_ = make_unique_ext<AdcConfigurationCborParser>();
    json_parser_ = make_unique_ext<AdcConfigurationJsonParser>();
    adc_manager_->Initialize();

    std::shared_ptr<IAdcManager> adc_manager = nullptr;

    try {
        adc_manager = Get(true);
    } catch(...) {
        LOG_ERR("Failed to load ADC configuration.");
    }

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

    ApplyJsonConfiguration();
}

bool AdcConfigurationManager::ApplyJsonConfiguration() {
    if(!json_configuration_service_->IsAvailable())
        return false;

    auto json_config_loaded = json_configuration_service_->Load();
    if(json_config_loaded.has_value()) {
        if(json_config_loaded->checksum == json_config_checksum_)
            return true;

        try {
            auto configuration = json_parser_->Deserialize(*json_config_loaded->config);

            json_config_checksum_ = json_config_loaded->checksum;

            if(!Update(configuration, true))
                return false;
        } catch(const std::exception& e) {
            LOG_ERR("Failed to deserialize JSON configuration. %s", e.what());
            return false;
        }

        LOG_INF("JSON configuration loaded successfully.");

        return true;
    }

    return true;
}

bool AdcConfigurationManager::Update(const AdcConfiguration& configuration, bool internal_only) {
    try {
        if(!internal_only && json_configuration_service_->IsAvailable()) {
            auto json_config = json_parser_->Serialize(configuration);
            json_configuration_service_->Save(json_config.get());

            auto json_config_loaded = json_configuration_service_->Load();
            if(!json_config_loaded.has_value()) {
                LOG_ERR("Failed to load newly updated JSON configuration.");
                return false;
            }

            LOG_INF("JSON configuration updated successfully.");

            json_config_checksum_ = json_config_loaded->checksum;
        }

        auto cbor_config = cbor_parser_->Serialize(configuration);
        cbor_config->json_config_checksum = json_config_checksum_;

        if(!cbor_configuration_service_->Save(cbor_config.get()))
            return false;
    } catch(const std::exception& e) {
        LOG_ERR("Failed to update ADC configuration. %s", e.what());
        return false;
    }

    Get(true);

    return true;
}

std::shared_ptr<IAdcManager> AdcConfigurationManager::Get(bool force_load) {
    if (configuration_ != nullptr && !force_load) {
        return adc_manager_;
    }

    auto cbor_config_data = cbor_configuration_service_->Load();
    if(!cbor_config_data.has_value())
        return nullptr;

    auto cbor_config = std::move(cbor_config_data.value().config);

    auto configuration = cbor_parser_->Deserialize(*cbor_config);
    configuration_ = std::make_shared<AdcConfiguration>(configuration);
    adc_manager_->UpdateConfiguration(configuration_);

    json_config_checksum_ = cbor_config->json_config_checksum;

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

    auto adc_calibration_data_samples_ptr = std::make_shared<std::vector<CalibrationData>>(adc_calibration_data_samples);
    auto adc_calibrator = std::make_shared<AdcCalibrator>(InterpolationMethod::LINEAR, adc_calibration_data_samples_ptr);

    std::vector<std::shared_ptr<AdcChannelConfiguration>> channel_configurations;
    channel_configurations.reserve(adc_manager_->GetChannelCount());
    for(size_t i = 0; i < adc_manager_->GetChannelCount(); ++i)
        channel_configurations.push_back(std::make_shared<AdcChannelConfiguration>(adc_calibrator));

    auto configuration = std::make_shared<AdcConfiguration>();
    configuration->samples = 40;
    configuration->channel_configurations =
        std::make_shared<std::vector<std::shared_ptr<AdcChannelConfiguration>>>(channel_configurations);

    return Update(*configuration);
}

} // namespace eerie_leap::domain::sensor_domain::configuration
