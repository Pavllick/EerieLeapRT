#include "utilities/memory/heap_allocator.hpp"
#include "utilities/cbor/cbor_helpers.hpp"
#include <memory>
#include "adc_configuration_controller.h"

namespace eerie_leap::controllers {

using namespace eerie_leap::utilities::memory;

bool AdcConfigurationController::Update(std::shared_ptr<AdcConfiguration> adc_configuration) {
    AdcConfig adc_config {
        .name = CborHelpers::ToZcborString(&adc_configuration->name),
        .samples = adc_configuration->samples
    };

    if(!adc_configuration_service_->Save(&adc_config))
        return false;

    adc_config_ = make_shared_ext<AdcConfig>(adc_config);
    adc_configuration_ = adc_configuration;

    return true;
}

std::shared_ptr<AdcConfiguration> AdcConfigurationController::Get() {
    if (adc_configuration_ != nullptr) {
        return adc_configuration_;
    }

    auto adc_config = adc_configuration_service_->Load();
    if(!adc_config.has_value())
        return nullptr;

    adc_config_raw_ = adc_config.value().config_raw;
    adc_config_ = adc_config.value().config;

    AdcConfiguration adc_configuration {
        .name = CborHelpers::ToStdString(adc_config_->name),
        .samples = static_cast<uint16_t>(adc_config_->samples)
    };
    adc_configuration_ = make_shared_ext<AdcConfiguration>(adc_configuration);

    return adc_configuration_;
}

} // namespace eerie_leap::controllers
