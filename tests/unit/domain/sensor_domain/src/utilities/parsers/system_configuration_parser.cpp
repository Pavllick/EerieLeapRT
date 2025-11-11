#include <zephyr/ztest.h>

#include "domain/system_domain/utilities/parsers/system_configuration_cbor_parser.h"
#include "domain/system_domain/utilities/parsers/system_configuration_json_parser.h"

using namespace eerie_leap::domain::system_domain::utilities::parsers;

ZTEST_SUITE(system_configuration_parser, NULL, NULL, NULL, NULL, NULL);

SystemConfiguration system_configuration_parser_GetTestConfiguration() {
    SystemConfiguration system_configuration {
        .device_id = 8765,
        .hw_version = 23456,
        .sw_version = 87654,
        .build_number = 12345,
        .com_user_refresh_rate_ms = 1000,

        .com_user_configurations {
            { .device_id = 1234, .server_id = 5678 },
            { .device_id = 5678, .server_id = 1234 }
        },

        .canbus_configurations {
            { .bus_channel = 0, .bitrate = 500000 },
            { .bus_channel = 1, .bitrate = 250000 },
            { .bus_channel = 2, .bitrate = 125000 }
        }
    };

    return system_configuration;
}

void system_configuration_parser_CompareSystemConfigurations(
    SystemConfiguration system_configuration,
    SystemConfiguration deserialized_system_configuration) {

    zassert_equal(deserialized_system_configuration.device_id, system_configuration.device_id);
    zassert_equal(deserialized_system_configuration.hw_version, system_configuration.hw_version);
    zassert_equal(deserialized_system_configuration.sw_version, system_configuration.sw_version);
    zassert_equal(deserialized_system_configuration.build_number, system_configuration.build_number);
    zassert_equal(deserialized_system_configuration.com_user_refresh_rate_ms, system_configuration.com_user_refresh_rate_ms);

    zassert_equal(deserialized_system_configuration.com_user_configurations.size(), system_configuration.com_user_configurations.size());

    for(int i = 0; i < system_configuration.com_user_configurations.size(); i++) {
        zassert_equal(deserialized_system_configuration.com_user_configurations[i].device_id, system_configuration.com_user_configurations[i].device_id);
        zassert_equal(deserialized_system_configuration.com_user_configurations[i].server_id, system_configuration.com_user_configurations[i].server_id);
    }

    zassert_equal(deserialized_system_configuration.canbus_configurations.size(), system_configuration.canbus_configurations.size());

    for(int i = 0; i < system_configuration.canbus_configurations.size(); i++) {
        zassert_equal(deserialized_system_configuration.canbus_configurations[i].bus_channel, system_configuration.canbus_configurations[i].bus_channel);
        zassert_equal(deserialized_system_configuration.canbus_configurations[i].bitrate, system_configuration.canbus_configurations[i].bitrate);
    }
}

ZTEST(system_configuration_parser, test_CborSerializeDeserialize) {
    SystemConfigurationCborParser system_configuration_cbor_parser;

    auto system_configuration = system_configuration_parser_GetTestConfiguration();

    auto serialized_system_configuration = system_configuration_cbor_parser.Serialize(system_configuration);
    auto deserialized_system_configuration = system_configuration_cbor_parser.Deserialize(*serialized_system_configuration.get());

    system_configuration_parser_CompareSystemConfigurations(system_configuration, deserialized_system_configuration);
}

ZTEST(system_configuration_parser, test_JsonSerializeDeserialize) {
    SystemConfigurationJsonParser system_configuration_json_parser;

    auto system_configuration = system_configuration_parser_GetTestConfiguration();

    auto serialized_system_configuration = system_configuration_json_parser.Serialize(system_configuration);
    auto deserialized_system_configuration = system_configuration_json_parser.Deserialize(*serialized_system_configuration.get());

    system_configuration_parser_CompareSystemConfigurations(system_configuration, deserialized_system_configuration);
}
