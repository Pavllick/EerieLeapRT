#include <algorithm>

#include "adc_json_parser.h"

namespace eerie_leap::domain::sensor_domain::utilities::parsers {

using namespace eerie_leap::utilities::memory;
using namespace eerie_leap::utilities::voltage_interpolator;
using namespace eerie_leap::domain::sensor_domain::utilities;

ext_unique_ptr<ExtVector> AdcJsonParser::Serialize(const AdcConfiguration& adc_configuration) {
    AdcConfigurationJsonDto adc_configuration_json;
    memset(&adc_configuration_json, 0, sizeof(AdcConfigurationJsonDto));

    adc_configuration_json.samples = adc_configuration.samples;

    for(size_t i = 0; i < adc_configuration.channel_configurations->size(); ++i) {
        AdcChannelConfigurationJsonDto channel_configuration;
        memset(&channel_configuration, 0, sizeof(AdcChannelConfigurationJsonDto));

        auto interpolation_method = adc_configuration.channel_configurations->at(i)->calibrator != nullptr
            ? adc_configuration.channel_configurations->at(i)->calibrator->GetInterpolationMethod()
            : InterpolationMethod::NONE;

        if(interpolation_method == InterpolationMethod::NONE)
            throw std::runtime_error("ADC channel configuration is invalid. Calibration table is missing.");

        channel_configuration.interpolation_method = GetInterpolationMethodName(interpolation_method);

        auto& calibration_table = *adc_configuration.channel_configurations->at(i)->calibrator->GetCalibrationTable();
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

        adc_configuration_json.channel_configurations[i] = channel_configuration;
        adc_configuration_json.channel_configurations_len++;
    }

    auto buf_size = json_calc_encoded_len(adc_configuration_descr, ARRAY_SIZE(adc_configuration_descr), &adc_configuration_json);
    if(buf_size < 0)
        throw std::runtime_error("Failed to calculate buffer size.");

    // Add 1 for null terminator
    auto buffer = make_unique_ext<ExtVector>(buf_size + 1);

    int res = json_obj_encode_buf(adc_configuration_descr, ARRAY_SIZE(adc_configuration_descr), &adc_configuration_json, (char*)buffer->data(), buffer->size());
    if(res != 0)
        throw std::runtime_error("Failed to serialize ADC configuration.");

    return buffer;
}

const AdcConfiguration AdcJsonParser::Deserialize(const std::span<const uint8_t>& json) {
	AdcConfigurationJsonDto adc_configuration_json;
	const int expected_return_code = BIT_MASK(ARRAY_SIZE(adc_configuration_descr));

	int ret = json_obj_parse((char*)json.data(), json.size(), adc_configuration_descr, ARRAY_SIZE(adc_configuration_descr), &adc_configuration_json);
	if(ret != expected_return_code)
        throw std::runtime_error("Invalid JSON payload.");

    AdcConfiguration adc_configuration;

    adc_configuration.samples = static_cast<uint16_t>(adc_configuration_json.samples);
    adc_configuration.channel_configurations = make_shared_ext<std::vector<std::shared_ptr<AdcChannelConfiguration>>>();

    for(size_t i = 0; i < adc_configuration_json.channel_configurations_len; ++i) {
        const auto& adc_channel_config = adc_configuration_json.channel_configurations[i];
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

        adc_configuration.channel_configurations->push_back(adc_channel_configuration);
    }

    return adc_configuration;
}

} // eerie_leap::domain::sensor_domain::utilities::parsers
