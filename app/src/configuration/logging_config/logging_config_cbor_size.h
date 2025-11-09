#pragma once

#include "utilities/cbor/cbor_size_builder.hpp"

#include "logging_config.h"

using namespace eerie_leap::utilities::cbor;

static size_t cbor_get_size_LoggingConfig(const LoggingConfig& input) {
    CborSizeBuilder builder;
    builder.AddIndefiniteArrayStart();

    builder.AddUint(input.logging_interval_ms)
        .AddUint(input.max_log_size_mb);

    builder.AddIndefiniteArrayStart();
    for(int i = 0; i < input.sensor_configurations.SensorLoggingConfig_m_count; i++) {
        builder.AddUint(input.sensor_configurations.SensorLoggingConfig_m[i].SensorLoggingConfig_m_key);

        builder.AddIndefiniteArrayStart();
        builder.AddBool(input.sensor_configurations.SensorLoggingConfig_m[i].SensorLoggingConfig_m.is_enabled)
            .AddBool(input.sensor_configurations.SensorLoggingConfig_m[i].SensorLoggingConfig_m.log_raw_value)
            .AddBool(input.sensor_configurations.SensorLoggingConfig_m[i].SensorLoggingConfig_m.log_only_new_data);
    }

    return builder.Build();
}
