#include <algorithm>

#include "adc_configuration_validator.h"
#include "adc_configuration_cbor_parser.h"

namespace eerie_leap::domain::sensor_domain::configuration::parsers {

pmr_unique_ptr<CborAdcConfig> AdcConfigurationCborParser::Serialize(const AdcConfiguration& configuration) {
    AdcConfigurationValidator::Validate(configuration);
    auto config = make_unique_pmr<CborAdcConfig>(Mrm::GetExtPmr());

    config->samples = configuration.samples;

    for(const auto& channel_configuration : *configuration.channel_configurations) {
        CborAdcChannelConfig adc_channel_config(std::allocator_arg, Mrm::GetExtPmr());

        auto interpolation_method = channel_configuration->calibrator != nullptr
            ? channel_configuration->calibrator->GetInterpolationMethod()
            : InterpolationMethod::NONE;

        adc_channel_config.interpolation_method = static_cast<uint32_t>(interpolation_method);
        if(interpolation_method != InterpolationMethod::NONE) {
            adc_channel_config.calibration_table_present = true;

            auto& calibration_table = *channel_configuration->calibrator->GetCalibrationTable();

            if(calibration_table.size() < 2)
                throw std::runtime_error("Calibration table must have at least 2 points.");

            std::ranges::sort(
                calibration_table,
                [](const CalibrationData& a, const CalibrationData& b) {
                    return a.voltage < b.voltage;
                });

            for(const auto& calibration_data : calibration_table) {
                adc_channel_config.calibration_table.float32float.push_back({
                    .float32float_key = calibration_data.voltage,
                    .float32float = calibration_data.value});
            }
        } else {
            throw std::runtime_error("ADC channel configuration is invalid. Calibration table is missing.");
        }

        config->CborAdcChannelConfig_m.push_back(std::move(adc_channel_config));
    }

    return config;
}

pmr_unique_ptr<AdcConfiguration> AdcConfigurationCborParser::Deserialize(std::pmr::memory_resource* mr, const CborAdcConfig& config) {
    auto configuration = make_unique_pmr<AdcConfiguration>(mr);

    configuration->samples = static_cast<uint16_t>(config.samples);
    configuration->channel_configurations = make_shared_pmr<std::vector<std::shared_ptr<AdcChannelConfiguration>>>(mr);

    for(const auto& adc_channel_config : config.CborAdcChannelConfig_m) {
        auto adc_channel_configuration = make_shared_pmr<AdcChannelConfiguration>(mr);

        auto interpolation_method = static_cast<InterpolationMethod>(adc_channel_config.interpolation_method);
        if(interpolation_method != InterpolationMethod::NONE && adc_channel_config.calibration_table_present) {
            std::pmr::vector<CalibrationData> calibration_table(mr);
            for(const auto& calibration_data : adc_channel_config.calibration_table.float32float) {
                calibration_table.push_back({
                    .voltage = calibration_data.float32float_key,
                    .value = calibration_data.float32float});
            }

            auto calibration_table_ptr = make_shared_pmr<std::pmr::vector<CalibrationData>>(mr, calibration_table);
            adc_channel_configuration->calibrator = make_shared_pmr<AdcCalibrator>(mr, interpolation_method, calibration_table_ptr);
        } else {
            throw std::runtime_error("ADC channel configuration is invalid. Calibration table is missing.");
        }

        configuration->channel_configurations->push_back(std::move(adc_channel_configuration));
    }

    AdcConfigurationValidator::Validate(*configuration);

    return configuration;
}

} // namespace eerie_leap::domain::sensor_domain::configuration::parsers
