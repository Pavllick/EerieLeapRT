#include <algorithm>
#include <vector>

#include "utilities/voltage_interpolator/calibration_data.h"

#include "adc_configuration_json_parser.h"

namespace eerie_leap::domain::sensor_domain::utilities::parsers {

using namespace eerie_leap::utilities::memory;
using namespace eerie_leap::utilities::voltage_interpolator;
using namespace eerie_leap::domain::sensor_domain::utilities;

ext_unique_ptr<JsonAdcConfig> AdcConfigurationJsonParser::Serialize(const AdcConfiguration& configuration) {
    auto config = make_unique_ext<JsonAdcConfig>();
    memset(config.get(), 0, sizeof(JsonAdcConfig));

    config->samples = configuration.samples;

    for(size_t i = 0; i < configuration.channel_configurations->size(); ++i) {
        JsonAdcChannelConfig channel_configuration;
        memset(&channel_configuration, 0, sizeof(JsonAdcChannelConfig));

        auto interpolation_method = configuration.channel_configurations->at(i)->calibrator != nullptr
            ? configuration.channel_configurations->at(i)->calibrator->GetInterpolationMethod()
            : InterpolationMethod::NONE;

        if(interpolation_method == InterpolationMethod::NONE)
            throw std::runtime_error("ADC channel configuration is invalid. Calibration table is missing.");

        channel_configuration.interpolation_method = GetInterpolationMethodName(interpolation_method);

        auto& calibration_table = *configuration.channel_configurations->at(i)->calibrator->GetCalibrationTable();
        channel_configuration.calibration_table_len = calibration_table.size();

        if(calibration_table.size() < 2)
            throw std::runtime_error("Calibration table must have at least 2 points.");

        std::ranges::sort(
            calibration_table,
            [](const CalibrationData& a, const CalibrationData& b) {
                return a.voltage < b.voltage;
            });

        for(size_t j = 0; j < calibration_table.size(); ++j) {
            const auto& calibration_data = calibration_table[j];
            channel_configuration.calibration_table[j].voltage = calibration_data.voltage;
            channel_configuration.calibration_table[j].value = calibration_data.value;
        }

        config->channel_configurations[i] = channel_configuration;
        config->channel_configurations_len++;
    }

    return config;
}

AdcConfiguration AdcConfigurationJsonParser::Deserialize(const JsonAdcConfig& config) {
    AdcConfiguration configuration;

    configuration.samples = static_cast<uint16_t>(config.samples);
    configuration.channel_configurations = make_shared_ext<std::vector<std::shared_ptr<AdcChannelConfiguration>>>();

    for(size_t i = 0; i < config.channel_configurations_len; ++i) {
        const auto& adc_channel_config = config.channel_configurations[i];
        auto adc_channel_configuration = make_shared_ext<AdcChannelConfiguration>();

        auto interpolation_method = GetInterpolationMethod(adc_channel_config.interpolation_method);

        if(interpolation_method == InterpolationMethod::NONE)
            throw std::runtime_error("ADC channel configuration is invalid. Calibration table is missing.");

        if(adc_channel_config.calibration_table_len < 2)
            throw std::runtime_error("Calibration table must have at least 2 points.");

        std::vector<CalibrationData> calibration_table;
        for(size_t j = 0; j < adc_channel_config.calibration_table_len; ++j) {
            const auto& calibration_data = adc_channel_config.calibration_table[j];

            calibration_table.push_back({
                .voltage = calibration_data.voltage,
                .value = calibration_data.value});
        }

        auto calibration_table_ptr = make_shared_ext<std::vector<CalibrationData>>(calibration_table);
        adc_channel_configuration->calibrator = make_shared_ext<AdcCalibrator>(interpolation_method, calibration_table_ptr);

        configuration.channel_configurations->push_back(adc_channel_configuration);
    }

    return configuration;
}

} // eerie_leap::domain::sensor_domain::utilities::parsers
