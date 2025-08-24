#include <memory>
#include <unordered_set>
#include <zephyr/ztest.h>

#include "utilities/voltage_interpolator/calibration_data.h"
#include "utilities/voltage_interpolator/linear_voltage_interpolator.hpp"

#include "subsys/adc/models/adc_configuration.h"
#include "subsys/adc/adc_simulator.h"
#include "subsys/adc/adc_emulator.h"

#include "domain/device_tree/dt_adc.h"

using namespace eerie_leap::subsys::adc;
using namespace eerie_leap::subsys::adc::models;
using namespace eerie_leap::domain::device_tree;

ZTEST_SUITE(adc, NULL, NULL, NULL, NULL, NULL);

std::shared_ptr<AdcConfiguration> adc_GetTestConfiguration() {
    std::vector<CalibrationData> adc_calibration_data_samples {
        {0.0, 0.0},
        {3.3, 3.3}
    };

    auto adc_calibration_data_samples_ptr = make_shared_ext<std::vector<CalibrationData>>(adc_calibration_data_samples);
    auto adc_calibrator = make_shared_ext<AdcCalibrator>(InterpolationMethod::LINEAR, adc_calibration_data_samples_ptr);

    std::vector<std::shared_ptr<AdcChannelConfiguration>> channel_configurations = {
        make_shared_ext<AdcChannelConfiguration>(adc_calibrator),
        make_shared_ext<AdcChannelConfiguration>(adc_calibrator),
        make_shared_ext<AdcChannelConfiguration>(adc_calibrator),
        make_shared_ext<AdcChannelConfiguration>(adc_calibrator)
    };

    auto adc_configuration = make_shared_ext<AdcConfiguration>();
    adc_configuration->samples = 40;
    adc_configuration->channel_configurations =
        make_shared_ext<std::vector<std::shared_ptr<AdcChannelConfiguration>>>(channel_configurations);

    return adc_configuration;
}

void ReadChannel_has_config(std::shared_ptr<IAdcManager> adc_manager) {
    auto adc_configuration = adc_GetTestConfiguration();

    adc_manager->UpdateConfiguration(adc_configuration);
    adc_manager->Initialize();

    auto channel_reader = adc_manager->GetChannelReader(2);

    std::unordered_set<float> readings;
    for(int j = 0; j < 100; j++) {
        float total = 0;

        for(int i = 0; i < 10; i++) {
            float reading = channel_reader();
            zassert_between_inclusive(reading, 0, 3.3);

            total += reading;
        }

        zassert_equal(readings.count(total), 0);
        readings.insert(total);
    }
}

ZTEST(adc, test_Simulator_ReadChannel_has_config) {
    auto adc = std::make_shared<AdcSimulatorManager>();
    ReadChannel_has_config(adc);
}

ZTEST(adc, test_Emulator_ReadChannel_has_config) {
    DtAdc::Initialize();

    auto adc = std::make_shared<AdcEmulatorManager>(DtAdc::Get().value());
    ReadChannel_has_config(adc);
}

void ReadChannel_no_config(std::shared_ptr<IAdcManager> adc_manager) {
    adc_manager->Initialize();

    try {
        auto channel_reader = adc_manager->GetChannelReader(2);
        channel_reader();
        zassert_true(true, "ReadChannel expected to fail, but it didn't.");
    } catch(...) {
        zassert_true(false, "ReadChannel failed as expected due to missing config.");
    }
}

ZTEST_EXPECT_FAIL(adc, test_Simulator_ReadChannel_no_config);
ZTEST(adc, test_Simulator_ReadChannel_no_config) {
    auto adc = std::make_shared<AdcSimulatorManager>();
    ReadChannel_no_config(adc);
}

ZTEST_EXPECT_FAIL(adc, test_Emulator_ReadChannel_no_config);
ZTEST(adc, test_Emulator_ReadChannel_no_config) {
    DtAdc::Initialize();

    auto adc = std::make_shared<AdcEmulatorManager>(DtAdc::Get().value());
    ReadChannel_no_config(adc);
}

void ReadChannel_out_of_range(std::shared_ptr<IAdcManager> adc_manager) {
    auto adc_configuration = adc_GetTestConfiguration();

    adc_manager->UpdateConfiguration(adc_configuration);
    adc_manager->Initialize();

    try {
        auto channel_reader = adc_manager->GetChannelReader(-2);
        channel_reader();
        zassert_true(true, "ReadChannel expected to fail, but it didn't.");
    } catch(...) {
        zassert_true(false, "ReadChannel failed as expected due to out of range channel.");
    }

    try {
        auto channel_reader = adc_manager->GetChannelReader(4);
        channel_reader();
        zassert_true(true, "ReadChannel expected to fail, but it didn't.");
    } catch(...) {
        zassert_true(false, "ReadChannel failed as expected due to out of range channel.");
    }
}

ZTEST_EXPECT_FAIL(adc, test_Simulator_ReadChannel_out_of_range);
ZTEST(adc, test_Simulator_ReadChannel_out_of_range) {
    auto adc = std::make_shared<AdcSimulatorManager>();
    ReadChannel_out_of_range(adc);
}

ZTEST_EXPECT_FAIL(adc, test_Emulator_ReadChannel_out_of_range);
ZTEST(adc, test_Emulator_ReadChannel_out_of_range) {
    DtAdc::Initialize();

    auto adc = std::make_shared<AdcEmulatorManager>(DtAdc::Get().value());
    ReadChannel_out_of_range(adc);
}
