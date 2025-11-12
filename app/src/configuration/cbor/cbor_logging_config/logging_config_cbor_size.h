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
    for(int i = 0; i < config.CborSensorLoggingConfig_m_count; i++) {
        builder.AddIndefiniteArrayStart()
            .AddUint(config.CborSensorLoggingConfig_m[i].sensor_id_hash)
            .AddBool(config.CborSensorLoggingConfig_m[i].is_enabled)
            .AddBool(config.CborSensorLoggingConfig_m[i].log_raw_value)
            .AddBool(config.CborSensorLoggingConfig_m[i].log_only_new_data);
    }

    builder.AddUint(config.json_config_checksum);

    return builder.Build();
}
