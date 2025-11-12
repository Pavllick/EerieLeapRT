#pragma once

#include "utilities/cbor/cbor_size_builder.hpp"

#include "system_config.h"

using namespace eerie_leap::utilities::cbor;

static size_t cbor_get_size_SystemConfig(const SystemConfig& system_config) {
    CborSizeBuilder builder;
    builder.AddIndefiniteArrayStart();

    builder.AddUint(system_config.device_id)
        .AddUint(system_config.hw_version)
        .AddUint(system_config.sw_version)
        .AddUint(system_config.build_number)
        .AddUint(system_config.com_user_refresh_rate_ms);

    builder.AddIndefiniteArrayStart();
    for(int i = 0; i < system_config.ComUserConfig_m_count; i++) {
        builder.AddIndefiniteArrayStart();

        builder.AddUint(system_config.ComUserConfig_m[i].device_id)
            .AddUint(system_config.ComUserConfig_m[i].server_id);
    }

    builder.AddIndefiniteArrayStart();
    for(int i = 0; i < system_config.CanbusConfig_m_count; i++) {
        builder.AddIndefiniteArrayStart();

        builder.AddUint(system_config.CanbusConfig_m[i].bus_channel)
            .AddUint(system_config.CanbusConfig_m[i].bitrate);
    }

    builder.AddUint(system_config.sd_json_checksum);

    return builder.Build();
}
