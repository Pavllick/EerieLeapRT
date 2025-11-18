#pragma once

#include "utilities/cbor/cbor_size_builder.hpp"

#include "cbor_canbus_config.h"

using namespace eerie_leap::utilities::cbor;

static size_t cbor_get_size_CborCanbusConfig(const CborCanbusConfig& config) {
    CborSizeBuilder builder;
    builder.AddIndefiniteArrayStart();

    builder.AddIndefiniteArrayStart();
    for(const auto& channel_configuration : config.CborCanChannelConfig_m) {
        builder.AddIndefiniteArrayStart();

        builder.AddUint(channel_configuration.type)
            .AddUint(channel_configuration.bus_channel)
            .AddUint(channel_configuration.bitrate);

        builder.AddIndefiniteArrayStart();
        for(const auto& message_configuration : channel_configuration.CborCanMessageConfig_m) {
            builder.AddIndefiniteArrayStart();

            builder.AddUint(message_configuration.frame_id)
                .AddUint(message_configuration.send_interval_ms);
        }
    }

    builder.AddUint(config.json_config_checksum);

    return builder.Build();
}
