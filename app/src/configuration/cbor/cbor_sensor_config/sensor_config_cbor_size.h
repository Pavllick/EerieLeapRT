#pragma once

#include "utilities/cbor/cbor_size_builder.hpp"

#include "cbor_sensor_config.h"

using namespace eerie_leap::utilities::cbor;

static size_t cbor_get_size_CborSensorsConfig(const CborSensorsConfig& config) {
    CborSizeBuilder builder;
    builder.AddIndefiniteArrayStart();

    builder.AddIndefiniteArrayStart();
    for(int i = 0; i < config.CborSensorConfig_m_count; i++) {
        builder.AddIndefiniteArrayStart();

        builder.AddTstr(config.CborSensorConfig_m[i].id);

        builder.AddIndefiniteArrayStart()
            .AddTstr(config.CborSensorConfig_m[i].metadata.name)
            .AddTstr(config.CborSensorConfig_m[i].metadata.unit)
            .AddTstr(config.CborSensorConfig_m[i].metadata.description);

        builder.AddIndefiniteArrayStart()
            .AddUint(config.CborSensorConfig_m[i].configuration.type)
            .AddUint(config.CborSensorConfig_m[i].configuration.sampling_rate_ms)
            .AddUint(config.CborSensorConfig_m[i].configuration.interpolation_method);


        builder.AddOptional(
            config.CborSensorConfig_m[i].configuration.channel_present,
            config.CborSensorConfig_m[i].configuration.channel,
            [](const auto& value) {

            return CborSizeCalc::SizeOfUint(value);
        });

        builder.AddTstr(config.CborSensorConfig_m[i].configuration.connection_string);

        builder.AddOptional(
            config.CborSensorConfig_m[i].configuration.calibration_table_present,
            config.CborSensorConfig_m[i].configuration.calibration_table,
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
            config.CborSensorConfig_m[i].configuration.expression_present,
            config.CborSensorConfig_m[i].configuration.expression,
            [](const auto& value) {

            return CborSizeCalc::SizeOfTstr(value);
        });
    }

    builder.AddUint(config.json_config_checksum);

    return builder.Build();
}
