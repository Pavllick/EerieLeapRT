#include "utilities/memory/heap_allocator.h"
#include "utilities/cbor/cbor_helpers.hpp"
#include <memory>
#include "adc_configuration_controller.h"

namespace eerie_leap::controllers {

using namespace eerie_leap::utilities::memory;


AdcConfigurationController::AdcConfigurationController(std::shared_ptr<ConfigurationService<AdcConfig>> adc_configuration_service) :
    adc_configuration_service_(std::move(adc_configuration_service)),
    adc_config_(nullptr),
    adc_configuration_(nullptr),
    logger_(Logger("AdcConfigCtrl")) {

    if(Get(true) == nullptr) {
        logger_.Error("Failed to load ADC configuration on init.");
    }
    else
        logger_.Info("Initialized successfully.");
}

bool AdcConfigurationController::Update(std::shared_ptr<AdcConfiguration> adc_configuration) {
    AdcConfig adc_config {
        .name = CborHelpers::ToZcborString(&adc_configuration->name),
        .samples = adc_configuration->samples
    };
    logger_.Debug("Attempting to save ADC configuration for name: %s, samples: %u", adc_configuration->name.c_str(), adc_configuration->samples);
    if(!adc_configuration_service_->Save(&adc_config)) {
        logger_.Error("Failed to save ADC configuration through service.");
        return false;
    }

    adc_config_ = make_shared_ext<AdcConfig>(adc_config);
    adc_configuration_ = adc_configuration;
    logger_.Info("Successfully updated and saved ADC configuration.");
    return true;
}

std::shared_ptr<AdcConfiguration> AdcConfigurationController::Get(bool force_load) {
    if (adc_configuration_ != nullptr && !force_load) {
        logger_.Debug("Returning cached ADC configuration.");
        return adc_configuration_;
    }

    logger_.Debug("Loading ADC configuration from service. Force load: %s", force_load ? "true" : "false");
    auto adc_config = adc_configuration_service_->Load();
    if(!adc_config.has_value()) { // Corrected adc_config_opt to adc_config
        logger_.Warning("Failed to load ADC configuration from service."); // Renamed warn to warning
        return nullptr;
    }

    adc_config_raw_ = adc_config.value().config_raw; // Corrected adc_config_opt to adc_config
    adc_config_ = adc_config.value().config; // Corrected adc_config_opt to adc_config

    AdcConfiguration adc_configuration {
        .name = CborHelpers::ToStdString(adc_config_->name),
        .samples = static_cast<uint16_t>(adc_config_->samples)
    };
    adc_configuration_ = make_shared_ext<AdcConfiguration>(adc_configuration);
    logger_.Info("Successfully loaded ADC configuration. Name: %s, Samples: %u", adc_configuration_->name.c_str(), adc_configuration_->samples);
    return adc_configuration_;
}

} // namespace eerie_leap::controllers
