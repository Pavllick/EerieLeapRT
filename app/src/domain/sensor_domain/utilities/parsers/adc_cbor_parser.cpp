#include <algorithm>

#include "adc_cbor_parser.h"

namespace eerie_leap::domain::sensor_domain::utilities::parsers {

ext_unique_ptr<AdcConfig> AdcCborParser::Serialize(const AdcConfiguration& adc_configuration) {
    auto adc_config = make_unique_ext<AdcConfig>();
    memset(adc_config.get(), 0, sizeof(AdcConfig));

    adc_config->samples = adc_configuration.samples;

    for(size_t i = 0; i < adc_configuration.channel_configurations->size(); ++i) {
        auto adc_channel_config = make_shared_ext<AdcChannelConfig>();
        memset(adc_channel_config.get(), 0, sizeof(AdcChannelConfig));

        auto interpolation_method = adc_configuration.channel_configurations->at(i)->calibrator != nullptr
            ? adc_configuration.channel_configurations->at(i)->calibrator->GetInterpolationMethod()
            : InterpolationMethod::NONE;

        adc_channel_config->interpolation_method = static_cast<uint32_t>(interpolation_method);
        if(interpolation_method != InterpolationMethod::NONE) {
            adc_channel_config->calibration_table_present = true;

            auto& calibration_table = *adc_configuration.channel_configurations->at(i)->calibrator->GetCalibrationTable();
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

    return adc_config;
}

const AdcConfiguration AdcCborParser::Deserialize(const AdcConfig& adc_config) {
    AdcConfiguration adc_configuration;

    adc_configuration.samples = static_cast<uint16_t>(adc_config.samples);
    adc_configuration.channel_configurations = make_shared_ext<std::vector<std::shared_ptr<AdcChannelConfiguration>>>();

    for(size_t i = 0; i < adc_config.AdcChannelConfig_m_count; ++i) {
        const auto& adc_channel_config = adc_config.AdcChannelConfig_m[i];
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

        adc_configuration.channel_configurations->push_back(adc_channel_configuration);
    }

    return adc_configuration;
}

} // namespace eerie_leap::domain::sensor_domain::utilities::parsers
