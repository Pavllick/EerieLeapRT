#pragma once

#include "utilities/cbor/cbor_size_builder.hpp"

#include "adc_config.h"

using namespace eerie_leap::utilities::cbor;

static size_t cbor_get_size_AdcConfig(const AdcConfig& input) {
    CborSizeBuilder builder;
    builder.AddIndefiniteArrayStart();

    builder.AddUint(input.samples);

    builder.AddIndefiniteArrayStart();
    for (size_t i = 0; i < input.AdcChannelConfig_m_count; i++) {
        builder.AddIndefiniteArrayStart();

        builder.AddUint(input.AdcChannelConfig_m[i].interpolation_method);

        builder.AddOptional(input.AdcChannelConfig_m[i].calibration_table_present,
            input.AdcChannelConfig_m[i].calibration_table,
            [](const AdcCalibrationDataMap& input) {

            CborSizeBuilder builder;
            builder.AddIndefiniteArrayStart();

            for(size_t i = 0; i < input.float32float_count; i++) {
                builder.AddFloat(input.float32float[i].float32float_key);
                builder.AddFloat(input.float32float[i].float32float);
            }

            return builder.Build();
        });
    }

    return builder.Build();
}
