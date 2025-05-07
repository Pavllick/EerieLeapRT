#include <memory>
#include <unordered_set>
#include <zephyr/ztest.h>

#include <stdio.h>

#include "domain/adc_domain/hardware/adc_configuration.h"
#include "domain/adc_domain/hardware/adc_simulator.h"
#include "domain/adc_domain/hardware/adc_emulator.h"

using namespace eerie_leap::domain::adc_domain::hardware;

ZTEST_SUITE(adc, NULL, NULL, NULL, NULL, NULL);

void ReadChannel_has_config(std::shared_ptr<IAdc> adc) {
    AdcConfiguration adc_config = {
        .samples = 1
    };

    adc->UpdateConfiguration(adc_config);
    adc->Initialize();

    std::unordered_set<double> readings;
    for(int j = 0; j < 100; j++) {
        double total = 0;

        for(int i = 0; i < 4; i++) {
            double reading = adc->ReadChannel(2);
            zassert_between_inclusive(reading, 0, 3.3);

            total += reading;
        }

        zassert_equal(readings.count(total), 0);
        readings.insert(total);
    }
}

ZTEST(adc, test_Simulator_ReadChannel_has_config) {
    auto adc = std::make_shared<AdcSimulator>();
    ReadChannel_has_config(adc);
}

ZTEST(adc, test_Emulator_ReadChannel_has_config) {
    auto adc = std::make_shared<AdcEmulator>();
    ReadChannel_has_config(adc);
}

void ReadChannel_no_config(std::shared_ptr<IAdc> adc) {
    adc->Initialize();

    try {
        adc->ReadChannel(2);
        zassert_true(true, "ReadChannel expected to fail, but it didn't.");
    } catch(...) {
        zassert_true(false, "ReadChannel failed as expected due to missing config.");
    }
}

ZTEST_EXPECT_FAIL(adc, test_Simulator_ReadChannel_no_config);
ZTEST(adc, test_Simulator_ReadChannel_no_config) {
    auto adc = std::make_shared<AdcSimulator>();
    ReadChannel_no_config(adc);
}

ZTEST_EXPECT_FAIL(adc, test_Emulator_ReadChannel_no_config);
ZTEST(adc, test_Emulator_ReadChannel_no_config) {
    auto adc = std::make_shared<AdcEmulator>();
    ReadChannel_no_config(adc);
}


void ReadChannel_out_of_range(std::shared_ptr<IAdc> adc) {
    AdcConfiguration adc_config = {
        .samples = 1
    };

    adc->UpdateConfiguration(adc_config);
    adc->Initialize();

    try {
        adc->ReadChannel(-2);
        zassert_true(true, "ReadChannel expected to fail, but it didn't.");
    } catch(...) {
        zassert_true(false, "ReadChannel failed as expected due to out of range channel.");
    }

    try {
        adc->ReadChannel(4);
        zassert_true(true, "ReadChannel expected to fail, but it didn't.");
    } catch(...) {
        zassert_true(false, "ReadChannel failed as expected due to out of range channel.");
    }
}

ZTEST_EXPECT_FAIL(adc, test_Simulator_ReadChannel_out_of_range);
ZTEST(adc, test_Simulator_ReadChannel_out_of_range) {
    auto adc = std::make_shared<AdcSimulator>();
    ReadChannel_out_of_range(adc);
}

ZTEST_EXPECT_FAIL(adc, test_Emulator_ReadChannel_out_of_range);
ZTEST(adc, test_Emulator_ReadChannel_out_of_range) {
    auto adc = std::make_shared<AdcEmulator>();
    ReadChannel_out_of_range(adc);
}
