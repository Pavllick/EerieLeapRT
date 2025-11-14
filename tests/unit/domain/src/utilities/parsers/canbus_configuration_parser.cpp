#include <zephyr/ztest.h>

#include "domain/canbus_domain/utilities/parsers/canbus_configuration_cbor_parser.h"
#include "domain/canbus_domain/utilities/parsers/canbus_configuration_json_parser.h"

using namespace eerie_leap::domain::canbus_domain::utilities::parsers;

ZTEST_SUITE(canbus_configuration_parser, NULL, NULL, NULL, NULL, NULL);

CanbusConfiguration canbus_configuration_parser_GetTestConfiguration() {
    CanbusConfiguration canbus_configuration {
        .channel_configurations {
            {
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
            },
            {
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
            },
            {
                .bus_channel = 2,
                .bitrate = 125000
            }
        }
    };

    return canbus_configuration;
}

void canbus_configuration_parser_CompareCanbusConfigurations(
    CanbusConfiguration canbus_configuration,
    CanbusConfiguration deserialized_configuration) {

    zassert_equal(deserialized_configuration.channel_configurations.size(), canbus_configuration.channel_configurations.size());

    for(int i = 0; i < canbus_configuration.channel_configurations.size(); i++) {
        zassert_equal(deserialized_configuration.channel_configurations[i].bus_channel, canbus_configuration.channel_configurations[i].bus_channel);
        zassert_equal(deserialized_configuration.channel_configurations[i].bitrate, canbus_configuration.channel_configurations[i].bitrate);

        zassert_equal(deserialized_configuration.channel_configurations[i].message_configurations.size(), canbus_configuration.channel_configurations[i].message_configurations.size());

        for(int j = 0; j < canbus_configuration.channel_configurations[i].message_configurations.size(); j++) {
            zassert_equal(deserialized_configuration.channel_configurations[i].message_configurations[j].frame_id, canbus_configuration.channel_configurations[i].message_configurations[j].frame_id);
            zassert_equal(deserialized_configuration.channel_configurations[i].message_configurations[j].send_interval_ms, canbus_configuration.channel_configurations[i].message_configurations[j].send_interval_ms);
        }
    }
}

ZTEST(canbus_configuration_parser, test_CborSerializeDeserialize) {
    CanbusConfigurationCborParser canbus_configuration_cbor_parser;

    auto canbus_configuration = canbus_configuration_parser_GetTestConfiguration();

    auto serialized_canbus_configuration = canbus_configuration_cbor_parser.Serialize(canbus_configuration);
    auto deserialized_canbus_configuration = canbus_configuration_cbor_parser.Deserialize(*serialized_canbus_configuration.get());

    canbus_configuration_parser_CompareCanbusConfigurations(canbus_configuration, deserialized_canbus_configuration);
}

ZTEST(canbus_configuration_parser, test_JsonSerializeDeserialize) {
    CanbusConfigurationJsonParser canbus_configuration_json_parser;

    auto canbus_configuration = canbus_configuration_parser_GetTestConfiguration();

    auto json_canbus_configuration = canbus_configuration_json_parser.Serialize(canbus_configuration);
    auto deserialized_canbus_configuration = canbus_configuration_json_parser.Deserialize(*json_canbus_configuration.get());

    canbus_configuration_parser_CompareCanbusConfigurations(canbus_configuration, deserialized_canbus_configuration);
}
