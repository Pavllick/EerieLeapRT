#pragma once

#include "utilities/cbor/cbor_size_builder.hpp"

#include "cbor_adc_config.h"

using namespace eerie_leap::utilities::cbor;

static size_t cbor_get_size_CborAdcConfig(const CborAdcConfig& config) {
    CborSizeBuilder builder;
    builder.AddIndefiniteArrayStart();

    builder.AddUint(config.samples);

    builder.AddIndefiniteArrayStart();
    for (const auto& channel_config : config.CborAdcChannelConfig_m) {
        builder.AddIndefiniteArrayStart();

        builder.AddUint(channel_config.interpolation_method);

        builder.AddOptional(channel_config.calibration_table_present,
            channel_config.calibration_table,
            [](const CborAdcCalibrationDataMap& calibration_table) {

            CborSizeBuilder builder;
            builder.AddIndefiniteArrayStart();

            for(const auto& calibration_data : calibration_table.float32float) {
                builder.AddFloat(calibration_data.float32float_key);
                builder.AddFloat(calibration_data.float32float);
            }

            return builder.Build();
        });
    }

    builder.AddUint(config.json_config_checksum);

    return builder.Build();
}
