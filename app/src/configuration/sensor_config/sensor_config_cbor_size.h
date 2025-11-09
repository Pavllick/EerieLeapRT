#pragma once

#include "utilities/cbor/cbor_size_builder.hpp"

#include "sensor_config.h"

using namespace eerie_leap::utilities::cbor;

static size_t cbor_get_size_SensorsConfig(const SensorsConfig& input) {
    CborSizeBuilder builder;
    builder.AddIndefiniteArrayStart();

    builder.AddIndefiniteArrayStart();
    for(int i = 0; i < input.SensorConfig_m_count; i++) {
        builder.AddIndefiniteArrayStart();

        builder.AddTstr(input.SensorConfig_m[i].id);

        builder.AddIndefiniteArrayStart()
            .AddTstr(input.SensorConfig_m[i].metadata.name)
            .AddTstr(input.SensorConfig_m[i].metadata.unit)
            .AddTstr(input.SensorConfig_m[i].metadata.description);

        builder.AddIndefiniteArrayStart()
            .AddUint(input.SensorConfig_m[i].configuration.type)
            .AddUint(input.SensorConfig_m[i].configuration.sampling_rate_ms)
            .AddUint(input.SensorConfig_m[i].configuration.interpolation_method);


        builder.AddOptional(
            input.SensorConfig_m[i].configuration.channel_present,
            input.SensorConfig_m[i].configuration.channel,
            [](const auto& value) {

            return CborSizeCalc::SizeOfUint(value);
        });

        builder.AddTstr(input.SensorConfig_m[i].configuration.connection_string);

        builder.AddOptional(
            input.SensorConfig_m[i].configuration.calibration_table_present,
            input.SensorConfig_m[i].configuration.calibration_table,
            [](const auto& value) {

            CborSizeBuilder builder;
            builder.AddIndefiniteArrayStart();

            for(int i = 0; i < value.float32float_count; i++) {
                builder.AddFloat(value.float32float[i].float32float_key)
                    .AddFloat(value.float32float[i].float32float);
            }

            return builder.Build();
        });

        builder.AddOptional(
            input.SensorConfig_m[i].configuration.expression_present,
            input.SensorConfig_m[i].configuration.expression,
            [](const auto& value) {

            return CborSizeCalc::SizeOfTstr(value);
        });
    }

    return builder.Build();
}
