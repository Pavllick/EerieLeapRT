#include "adc_configuration_validator.h"

namespace eerie_leap::domain::sensor_domain::configuration::parsers {

static void InvalidConfiguration(uint32_t channel, std::string_view message) {
    throw std::invalid_argument(
        "Invalid ADC configuration. Channel: "
        + std::to_string(channel)
        + ". "
        + std::string(message));
}

void AdcConfigurationValidator::ValidateSamples(const AdcConfiguration& configuration) {
    if(configuration.samples == 0)
        throw std::invalid_argument(
            "Invalid ADC configuration. Samples must be greater than 0.");
}

void AdcConfigurationValidator::ValidateChannels(const AdcConfiguration& configuration) {
    for(int i = 0; i < configuration.channel_configurations->size(); i++) {
        auto& channel_configuration = configuration.channel_configurations->at(i);

        if(channel_configuration->calibrator == nullptr)
            InvalidConfiguration(i, "Invalid interpolation method.");

        const auto& calibration_table = *channel_configuration->calibrator->GetCalibrationTable();

        if(calibration_table.size() < 2)
            InvalidConfiguration(i, "Calibration table must have at least 2 points.");
    }
}

void AdcConfigurationValidator::Validate(const AdcConfiguration& configuration) {
    ValidateSamples(configuration);
    ValidateChannels(configuration);
}

} // namespace eerie_leap::domain::sensor_domain::configuration::parsers
