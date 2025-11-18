#include <algorithm>

#include "adc_configuration_cbor_parser.h"

namespace eerie_leap::domain::sensor_domain::utilities::parsers {

ext_unique_ptr<CborAdcConfig> AdcConfigurationCborParser::Serialize(const AdcConfiguration& adc_configuration) {
    auto adc_config = make_unique_ext<CborAdcConfig>();

    adc_config->samples = adc_configuration.samples;

    for(const auto& channel_configuration : *adc_configuration.channel_configurations) {
        CborAdcChannelConfig adc_channel_config;

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

        adc_config->CborAdcChannelConfig_m.push_back(adc_channel_config);
    }

    return adc_config;
}

AdcConfiguration AdcConfigurationCborParser::Deserialize(const CborAdcConfig& adc_config) {
    AdcConfiguration adc_configuration;

    adc_configuration.samples = static_cast<uint16_t>(adc_config.samples);
    adc_configuration.channel_configurations = make_shared_ext<std::vector<std::shared_ptr<AdcChannelConfiguration>>>();

    for(const auto& adc_channel_config : adc_config.CborAdcChannelConfig_m) {
        auto adc_channel_configuration = make_shared_ext<AdcChannelConfiguration>();

        auto interpolation_method = static_cast<InterpolationMethod>(adc_channel_config.interpolation_method);
        if(interpolation_method != InterpolationMethod::NONE && adc_channel_config.calibration_table_present) {
            std::vector<CalibrationData> calibration_table;
            for(const auto& calibration_data : adc_channel_config.calibration_table.float32float) {
                calibration_table.push_back({
                    .voltage = calibration_data.float32float_key,
                    .value = calibration_data.float32float});
            }

            auto calibration_table_ptr = make_shared_ext<std::vector<CalibrationData>>(calibration_table);
            adc_channel_configuration->calibrator = make_shared_ext<AdcCalibrator>(interpolation_method, calibration_table_ptr);
        } else {
            throw std::runtime_error("ADC channel configuration is invalid. Calibration table is missing.");
        }

        adc_configuration.channel_configurations->push_back(adc_channel_configuration);
    }

    return adc_configuration;
}

} // namespace eerie_leap::domain::sensor_domain::utilities::parsers
