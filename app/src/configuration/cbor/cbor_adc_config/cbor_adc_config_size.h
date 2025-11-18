#pragma once

#include "utilities/cbor/cbor_size_builder.hpp"

#include "cbor_adc_config.h"

using namespace eerie_leap::utilities::cbor;

static size_t cbor_get_size_CborAdcConfig(const CborAdcConfig& config) {
    CborSizeBuilder builder;
    builder.AddIndefiniteArrayStart();

    builder.AddUint(config.samples);

    builder.AddIndefiniteArrayStart();
    for (size_t i = 0; i < config.CborAdcChannelConfig_m_count; i++) {
        builder.AddIndefiniteArrayStart();

        builder.AddUint(config.CborAdcChannelConfig_m[i].interpolation_method);

        builder.AddOptional(config.CborAdcChannelConfig_m[i].calibration_table_present,
            config.CborAdcChannelConfig_m[i].calibration_table,
            [](const CborAdcCalibrationDataMap& config) {

            CborSizeBuilder builder;
            builder.AddIndefiniteArrayStart();

            for(size_t i = 0; i < config.float32float_count; i++) {
                builder.AddFloat(config.float32float[i].float32float_key);
                builder.AddFloat(config.float32float[i].float32float);
            }

            return builder.Build();
        });
    }

    builder.AddUint(config.json_config_checksum);

    return builder.Build();
}
