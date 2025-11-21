#include <zephyr/ztest.h>

#include "domain/canbus_domain/configuration/utilities/parsers/canbus_configuration_cbor_parser.h"
#include "domain/canbus_domain/configuration/utilities/parsers/canbus_configuration_json_parser.h"

using namespace eerie_leap::domain::canbus_domain::configuration::utilities::parsers;

ZTEST_SUITE(canbus_configuration_parser, NULL, NULL, NULL, NULL, NULL);

CanbusConfiguration canbus_configuration_parser_GetTestConfiguration() {
    CanbusConfiguration canbus_configuration;
    CanChannelConfiguration channel_configuration_0 {
        .bus_channel = 0,
        .bitrate = 500000,
        .message_configurations {
            {
                .frame_id = 0x123,
                .send_interval_ms = 100
            },
            {
                .frame_id = 0x456,
                .send_interval_ms = 200
            }
        }
    };
    CanChannelConfiguration channel_configuration_1 {
        .bus_channel = 1,
        .bitrate = 250000,
        .message_configurations {
            {
                .frame_id = 0x123,
                .send_interval_ms = 100
            },
            {
                .frame_id = 0x456,
                .send_interval_ms = 200
            },
            {
                .frame_id = 0x789,
                .send_interval_ms = 300
            }
        }
    };
    CanChannelConfiguration channel_configuration_2 {
        .bus_channel = 2,
        .bitrate = 125000
    };

    canbus_configuration.channel_configurations.emplace(channel_configuration_0.bus_channel, channel_configuration_0);
    canbus_configuration.channel_configurations.emplace(channel_configuration_1.bus_channel, channel_configuration_1);
    canbus_configuration.channel_configurations.emplace(channel_configuration_2.bus_channel, channel_configuration_2);

    return canbus_configuration;
}

void canbus_configuration_parser_CompareCanbusConfigurations(
    CanbusConfiguration canbus_configuration,
    CanbusConfiguration deserialized_configuration) {

    zassert_equal(deserialized_configuration.channel_configurations.size(), canbus_configuration.channel_configurations.size());

    for(auto [bus_channel, channel_configuration] : canbus_configuration.channel_configurations) {
        zassert_equal(deserialized_configuration.channel_configurations[bus_channel].bus_channel, channel_configuration.bus_channel);
        zassert_equal(deserialized_configuration.channel_configurations[bus_channel].bitrate, channel_configuration.bitrate);

        zassert_equal(deserialized_configuration.channel_configurations[bus_channel].message_configurations.size(), channel_configuration.message_configurations.size());

        for(int j = 0; j < channel_configuration.message_configurations.size(); j++) {
            zassert_equal(deserialized_configuration.channel_configurations[bus_channel].message_configurations[j].frame_id, channel_configuration.message_configurations[j].frame_id);
            zassert_equal(deserialized_configuration.channel_configurations[bus_channel].message_configurations[j].send_interval_ms, channel_configuration.message_configurations[j].send_interval_ms);
        }
    }
}

ZTEST(canbus_configuration_parser, test_CborSerializeDeserialize) {
    CanbusConfigurationCborParser canbus_configuration_cbor_parser(nullptr);

    auto canbus_configuration = canbus_configuration_parser_GetTestConfiguration();

    auto serialized_canbus_configuration = canbus_configuration_cbor_parser.Serialize(canbus_configuration);
    auto deserialized_canbus_configuration = canbus_configuration_cbor_parser.Deserialize(*serialized_canbus_configuration.get());

    canbus_configuration_parser_CompareCanbusConfigurations(canbus_configuration, deserialized_canbus_configuration);
}

ZTEST(canbus_configuration_parser, test_JsonSerializeDeserialize) {
    CanbusConfigurationJsonParser canbus_configuration_json_parser(nullptr);

    auto canbus_configuration = canbus_configuration_parser_GetTestConfiguration();

    auto json_canbus_configuration = canbus_configuration_json_parser.Serialize(canbus_configuration);
    auto deserialized_canbus_configuration = canbus_configuration_json_parser.Deserialize(*json_canbus_configuration.get());

    canbus_configuration_parser_CompareCanbusConfigurations(canbus_configuration, deserialized_canbus_configuration);
}
