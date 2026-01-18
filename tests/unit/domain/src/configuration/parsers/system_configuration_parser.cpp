#include <zephyr/ztest.h>

#include "domain/system_domain/configuration/parsers/system_configuration_cbor_parser.h"

using namespace eerie_leap::domain::system_domain::configuration::parsers;

ZTEST_SUITE(system_configuration_parser, NULL, NULL, NULL, NULL, NULL);

SystemConfiguration system_configuration_parser_GetTestConfiguration() {
    SystemConfiguration system_configuration(std::allocator_arg, Mrm::GetDefaultPmr());
    system_configuration.device_id = 8765;
    system_configuration.hw_version = 23456;
    system_configuration.sw_version = 87654;
    system_configuration.build_number = 12345;

    return system_configuration;
}

void system_configuration_parser_CompareSystemConfigurations(
    SystemConfiguration& system_configuration,
    SystemConfiguration& deserialized_system_configuration) {

    zassert_equal(deserialized_system_configuration.device_id, system_configuration.device_id);
    zassert_equal(deserialized_system_configuration.hw_version, system_configuration.hw_version);
    zassert_equal(deserialized_system_configuration.sw_version, system_configuration.sw_version);
    zassert_equal(deserialized_system_configuration.build_number, system_configuration.build_number);
}

ZTEST(system_configuration_parser, test_CborSerializeDeserialize) {
    SystemConfigurationCborParser system_configuration_cbor_parser;

    auto system_configuration = system_configuration_parser_GetTestConfiguration();

    auto serialized_system_configuration = system_configuration_cbor_parser.Serialize(system_configuration);
    auto deserialized_system_configuration = system_configuration_cbor_parser.Deserialize(Mrm::GetDefaultPmr(), *serialized_system_configuration.get());

    system_configuration_parser_CompareSystemConfigurations(system_configuration, *deserialized_system_configuration);
}
