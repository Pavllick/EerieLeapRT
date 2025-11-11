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
    for(int i = 0; i < input.SensorLoggingConfig_m_count; i++) {
        builder.AddIndefiniteArrayStart()
            .AddUint(input.SensorLoggingConfig_m[i].sensor_id_hash)
            .AddBool(input.SensorLoggingConfig_m[i].is_enabled)
            .AddBool(input.SensorLoggingConfig_m[i].log_raw_value)
            .AddBool(input.SensorLoggingConfig_m[i].log_only_new_data);
    }

    return builder.Build();
}
