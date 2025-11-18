#pragma once

#include "utilities/cbor/cbor_size_builder.hpp"

#include "cbor_system_config.h"

using namespace eerie_leap::utilities::cbor;

static size_t cbor_get_size_CborSystemConfig(const CborSystemConfig& config) {
    CborSizeBuilder builder;
    builder.AddIndefiniteArrayStart();

    builder.AddUint(config.device_id)
        .AddUint(config.hw_version)
        .AddUint(config.sw_version)
        .AddUint(config.build_number)
        .AddUint(config.com_user_refresh_rate_ms);

    builder.AddIndefiniteArrayStart();
    for(int i = 0; i < config.CborComUserConfig_m_count; i++) {
        builder.AddIndefiniteArrayStart();

        builder.AddUint(config.CborComUserConfig_m[i].device_id)
            .AddUint(config.CborComUserConfig_m[i].server_id);
    }

    builder.AddUint(config.json_config_checksum);

    return builder.Build();
}
