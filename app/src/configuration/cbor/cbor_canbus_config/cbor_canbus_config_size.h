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
            .AddBool(channel_configuration.is_extended_id)
            .AddUint(channel_configuration.bus_channel)
            .AddUint(channel_configuration.bitrate)
            .AddUint(channel_configuration.data_bitrate)
            .AddTstr(channel_configuration.dbc_file_path);

        builder.AddIndefiniteArrayStart();
        for(const auto& message_configuration : channel_configuration.CborCanMessageConfig_m) {
            builder.AddIndefiniteArrayStart();

            builder.AddUint(message_configuration.frame_id)
                .AddUint(message_configuration.send_interval_ms)
                .AddTstr(message_configuration.script_path)
                .AddTstr(message_configuration.name)
                .AddUint(message_configuration.message_size);

            builder.AddIndefiniteArrayStart();
            for(const auto& signal_configuration : message_configuration.CborCanSignalConfig_m) {
                builder.AddIndefiniteArrayStart();

                builder.AddUint(signal_configuration.start_bit)
                    .AddUint(signal_configuration.size_bits)
                    .AddFloat(signal_configuration.factor)
                    .AddFloat(signal_configuration.offset)
                    .AddTstr(signal_configuration.name)
                    .AddTstr(signal_configuration.unit);
            }

        }
    }

    builder.AddUint(config.json_config_checksum);

    return builder.Build();
}
