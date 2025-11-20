#pragma once

#include "utilities/cbor/cbor_size_builder.hpp"

#include "cbor_sensors_config.h"

using namespace eerie_leap::utilities::cbor;

static size_t cbor_get_size_CborSensorsConfig(const CborSensorsConfig& config) {
    CborSizeBuilder builder;
    builder.AddIndefiniteArrayStart();

    builder.AddIndefiniteArrayStart();
    for(const auto& sensor_config : config.CborSensorConfig_m) {
        builder.AddIndefiniteArrayStart();

        builder.AddTstr(sensor_config.id);

        builder.AddIndefiniteArrayStart()
            .AddTstr(sensor_config.metadata.name)
            .AddTstr(sensor_config.metadata.unit)
            .AddTstr(sensor_config.metadata.description);

        builder.AddIndefiniteArrayStart()
            .AddUint(sensor_config.configuration.type)
            .AddUint(sensor_config.configuration.sampling_rate_ms)
            .AddUint(sensor_config.configuration.interpolation_method);


        builder.AddOptional(
            sensor_config.configuration.channel_present,
            sensor_config.configuration.channel,
            [](const auto& value) {

            return CborSizeCalc::SizeOfUint(value);
        });

        builder.AddTstr(sensor_config.configuration.connection_string);
        builder.AddTstr(sensor_config.configuration.script_path);

        builder.AddOptional(
            sensor_config.configuration.calibration_table_present,
            sensor_config.configuration.calibration_table,
            [](const auto& value) {

            CborSizeBuilder builder;
            builder.AddIndefiniteArrayStart();

            for(const auto& calibration_data : value.float32float) {
                builder.AddFloat(calibration_data.float32float_key)
                    .AddFloat(calibration_data.float32float);
            }

            return builder.Build();
        });

        builder.AddOptional(
            sensor_config.configuration.expression_present,
            sensor_config.configuration.expression,
            [](const auto& value) {

            return CborSizeCalc::SizeOfTstr(value);
        });
    }

    builder.AddUint(config.json_config_checksum);

    return builder.Build();
}
