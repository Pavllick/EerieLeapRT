#pragma once

#include "utilities/cbor/cbor_size_builder.hpp"

#include "cbor_logging_config.h"

using namespace eerie_leap::utilities::cbor;

static size_t cbor_get_size_CborLoggingConfig(const CborLoggingConfig& config) {
    CborSizeBuilder builder;
    builder.AddIndefiniteArrayStart();

    builder.AddUint(config.logging_interval_ms)
        .AddUint(config.max_log_size_mb);

    builder.AddIndefiniteArrayStart();
    for(const auto& sensor_logging_config : config.CborSensorLoggingConfig_m) {
        builder.AddIndefiniteArrayStart()
            .AddUint(sensor_logging_config.sensor_id_hash)
            .AddBool(sensor_logging_config.is_enabled)
            .AddBool(sensor_logging_config.log_raw_value)
            .AddBool(sensor_logging_config.log_only_new_data);
    }

    builder.AddUint(config.json_config_checksum);

    return builder.Build();
}
