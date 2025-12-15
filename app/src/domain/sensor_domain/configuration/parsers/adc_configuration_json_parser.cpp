#include <algorithm>
#include <vector>

#include "utilities/voltage_interpolator/calibration_data.h"

#include "adc_configuration_validator.h"
#include "adc_configuration_json_parser.h"

namespace eerie_leap::domain::sensor_domain::configuration::parsers {

using namespace eerie_leap::utilities::memory;
using namespace eerie_leap::utilities::voltage_interpolator;

pmr_unique_ptr<JsonAdcConfig> AdcConfigurationJsonParser::Serialize(const AdcConfiguration& configuration) {
    AdcConfigurationValidator::Validate(configuration);

    auto config = make_unique_pmr<JsonAdcConfig>(Mrm::GetExtPmr());

    config->samples = configuration.samples;

    for(size_t i = 0; i < configuration.channel_configurations->size(); ++i) {
        JsonAdcChannelConfig channel_configuration;

        auto interpolation_method = configuration.channel_configurations->at(i)->calibrator != nullptr
            ? configuration.channel_configurations->at(i)->calibrator->GetInterpolationMethod()
            : InterpolationMethod::NONE;

        if(interpolation_method == InterpolationMethod::NONE)
            throw std::runtime_error("ADC channel configuration is invalid. Calibration table is missing.");

        channel_configuration.interpolation_method = json::string(GetInterpolationMethodName(interpolation_method));

        auto& calibration_table = *configuration.channel_configurations->at(i)->calibrator->GetCalibrationTable();

        if(calibration_table.size() < 2)
            throw std::runtime_error("Calibration table must have at least 2 points.");

        std::ranges::sort(
            calibration_table,
            [](const CalibrationData& a, const CalibrationData& b) {
                return a.voltage < b.voltage;
            });

        for(auto& calibration_data : calibration_table) {
            channel_configuration.calibration_table.push_back({
                .voltage = calibration_data.voltage,
                .value = calibration_data.value});
        }

        config->channel_configs.push_back(std::move(channel_configuration));
    }

    return config;
}

pmr_unique_ptr<AdcConfiguration> AdcConfigurationJsonParser::Deserialize(std::pmr::memory_resource* mr, const JsonAdcConfig& config) {
    auto configuration = make_unique_pmr<AdcConfiguration>(mr);

    configuration->samples = static_cast<uint16_t>(config.samples);
    configuration->channel_configurations = make_shared_pmr<std::vector<std::shared_ptr<AdcChannelConfiguration>>>(mr);

    for(auto& adc_channel_config : config.channel_configs) {
        auto adc_channel_configuration = make_shared_pmr<AdcChannelConfiguration>(mr);

        auto interpolation_method = GetInterpolationMethod(std::string(adc_channel_config.interpolation_method));

        if(interpolation_method == InterpolationMethod::NONE)
            throw std::runtime_error("ADC channel configuration is invalid. Calibration table is missing.");

        if(adc_channel_config.calibration_table.size() < 2)
            throw std::runtime_error("Calibration table must have at least 2 points.");

        std::pmr::vector<CalibrationData> calibration_table(mr);
        for(auto& calibration_data : adc_channel_config.calibration_table) {
            calibration_table.push_back({
                .voltage = calibration_data.voltage,
                .value = calibration_data.value});
        }

        auto calibration_table_ptr = make_shared_pmr<std::pmr::vector<CalibrationData>>(mr, calibration_table);
        adc_channel_configuration->calibrator = make_shared_pmr<AdcCalibrator>(mr, interpolation_method, calibration_table_ptr);

        configuration->channel_configurations->push_back(std::move(adc_channel_configuration));
    }

    AdcConfigurationValidator::Validate(*configuration);

    return configuration;
}

} // eerie_leap::domain::sensor_domain::configuration::parsers
