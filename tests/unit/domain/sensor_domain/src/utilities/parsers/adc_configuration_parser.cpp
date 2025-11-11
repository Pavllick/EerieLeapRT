#include <zephyr/ztest.h>

#include "domain/sensor_domain/utilities/parsers/adc_configuration_cbor_parser.h"
#include "domain/sensor_domain/utilities/parsers/adc_configuration_json_parser.h"

using namespace eerie_leap::domain::sensor_domain::utilities::parsers;

ZTEST_SUITE(adc_configuration_parser, NULL, NULL, NULL, NULL, NULL);

AdcConfiguration adc_configuration_parser_GetTestConfiguration() {
    std::vector<CalibrationData> adc_calibration_data_samples_1 {
        {0.0, 0.0},
        {5.0, 5.0}
    };

    auto adc_calibration_data_samples_ptr_1 = make_shared_ext<std::vector<CalibrationData>>(adc_calibration_data_samples_1);
    auto adc_calibrator_1 = make_shared_ext<AdcCalibrator>(InterpolationMethod::LINEAR, adc_calibration_data_samples_ptr_1);

    std::vector<CalibrationData> adc_calibration_data_samples_2 {
        {0.501, 0.469},
        {1.0, 0.968},
        {2.0, 1.970},
        {3.002, 2.98},
        {4.003, 4.01},
        {5.0, 5.0}
    };

    auto adc_calibration_data_samples_ptr_2 = make_shared_ext<std::vector<CalibrationData>>(adc_calibration_data_samples_2);
    auto adc_calibrator_2 = make_shared_ext<AdcCalibrator>(InterpolationMethod::CUBIC_SPLINE, adc_calibration_data_samples_ptr_2);

    std::vector<std::shared_ptr<AdcChannelConfiguration>> channel_configurations = {
        make_shared_ext<AdcChannelConfiguration>(adc_calibrator_1),
        make_shared_ext<AdcChannelConfiguration>(adc_calibrator_2),
        make_shared_ext<AdcChannelConfiguration>(adc_calibrator_1),
        make_shared_ext<AdcChannelConfiguration>(adc_calibrator_1),
        make_shared_ext<AdcChannelConfiguration>(adc_calibrator_1),
        make_shared_ext<AdcChannelConfiguration>(adc_calibrator_2),
        make_shared_ext<AdcChannelConfiguration>(adc_calibrator_2),
        make_shared_ext<AdcChannelConfiguration>(adc_calibrator_2),
    };

    AdcConfiguration adc_configuration;
    adc_configuration.samples = 40;
    adc_configuration.channel_configurations =
        make_shared_ext<std::vector<std::shared_ptr<AdcChannelConfiguration>>>(channel_configurations);

    return adc_configuration;
}

void adc_configuration_parser_CompareAdcConfigurations(AdcConfiguration adc_configuration, AdcConfiguration deserialized_adc_configuration) {
    zassert_equal(deserialized_adc_configuration.samples, adc_configuration.samples);
    zassert_equal(deserialized_adc_configuration.channel_configurations->size(), adc_configuration.channel_configurations->size());

    for(int i = 0; i < adc_configuration.channel_configurations->size(); i++) {
        auto adc_channel_configuration = adc_configuration.channel_configurations->at(i);
        auto deserialized_adc_channel_configuration = deserialized_adc_configuration.channel_configurations->at(i);

        zassert_equal(deserialized_adc_channel_configuration->calibrator->GetInterpolationMethod(), adc_channel_configuration->calibrator->GetInterpolationMethod());

        auto calibration_table = adc_channel_configuration->calibrator->GetCalibrationTable();
        auto deserialized_calibration_table = deserialized_adc_channel_configuration->calibrator->GetCalibrationTable();
        zassert_equal(calibration_table->size(), deserialized_calibration_table->size());

        for(int j = 0; j < calibration_table->size(); j++) {
            zassert_equal(calibration_table->at(j).voltage, deserialized_calibration_table->at(j).voltage);
            zassert_equal(calibration_table->at(j).value, deserialized_calibration_table->at(j).value);
        }
    }
}

ZTEST(adc_configuration_parser, test_CborSerializeDeserialize) {
    AdcConfigurationCborParser adc_configuration_cbor_parser;

    auto adc_configuration = adc_configuration_parser_GetTestConfiguration();

    auto serialized_adc_configuration = adc_configuration_cbor_parser.Serialize(adc_configuration);
    auto deserialized_adc_configuration = adc_configuration_cbor_parser.Deserialize(*serialized_adc_configuration.get());

    adc_configuration_parser_CompareAdcConfigurations(adc_configuration, deserialized_adc_configuration);
}

ZTEST(adc_configuration_parser, test_JsonSerializeDeserialize) {
    AdcConfigurationJsonParser adc_configuration_json_parser;

    auto adc_configuration = adc_configuration_parser_GetTestConfiguration();

    auto serialized_adc_configuration = adc_configuration_json_parser.Serialize(adc_configuration);
    auto deserialized_adc_configuration = adc_configuration_json_parser.Deserialize(*serialized_adc_configuration.get());

    adc_configuration_parser_CompareAdcConfigurations(adc_configuration, deserialized_adc_configuration);
}
