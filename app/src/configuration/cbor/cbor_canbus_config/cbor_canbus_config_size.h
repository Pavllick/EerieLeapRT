#pragma once

#include "utilities/cbor/cbor_size_builder.hpp"

#include "cbor_canbus_config.h"

using namespace eerie_leap::utilities::cbor;

static size_t cbor_get_size_CborCanbusConfig(const CborCanbusConfig& config) {
    CborSizeBuilder builder;
    builder.AddIndefiniteArrayStart();

    builder.AddIndefiniteArrayStart();
    for(int i = 0; i < config.CborCanChannelConfig_m_count; i++) {
        builder.AddIndefiniteArrayStart();

        builder.AddUint(config.CborCanChannelConfig_m[i].type)
            .AddUint(config.CborCanChannelConfig_m[i].bus_channel)
            .AddUint(config.CborCanChannelConfig_m[i].bitrate);

        builder.AddIndefiniteArrayStart();
        for(int j = 0; j < config.CborCanChannelConfig_m[i].CborCanMessageConfig_m_count; j++) {
            builder.AddIndefiniteArrayStart();

            builder.AddUint(config.CborCanChannelConfig_m[i].CborCanMessageConfig_m[j].frame_id)
                .AddUint(config.CborCanChannelConfig_m[i].CborCanMessageConfig_m[j].send_interval_ms);
        }
    }

    builder.AddUint(config.json_config_checksum);

    return builder.Build();
}
