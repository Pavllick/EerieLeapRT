#include <zephyr/ztest.h>

#include "domain/logging_domain/configuration/utilities/parsers/logging_configuration_cbor_parser.h"
#include "domain/logging_domain/configuration/utilities/parsers/logging_configuration_json_parser.h"

using namespace eerie_leap::domain::logging_domain::configuration::utilities::parsers;

ZTEST_SUITE(logging_configuration_parser, NULL, NULL, NULL, NULL, NULL);

LoggingConfiguration logging_configuration_parser_GetTestConfiguration() {
    LoggingConfiguration logging_configuration {
        .logging_interval_ms = 1000,
        .max_log_size_mb = 1
    };

    logging_configuration.sensor_configurations.emplace(1234, SensorLoggingConfiguration {
        .is_enabled = true,
        .log_raw_value = true,
        .log_only_new_data = true
    });
    logging_configuration.sensor_configurations.emplace(5678, SensorLoggingConfiguration {
        .is_enabled = false,
        .log_raw_value = false,
        .log_only_new_data = false
    });

    return logging_configuration;
}

void logging_configuration_parser_CompareLoggingConfigurations(
    LoggingConfiguration logging_configuration,
    LoggingConfiguration deserialized_logging_configuration) {

    zassert_equal(deserialized_logging_configuration.logging_interval_ms, logging_configuration.logging_interval_ms);
    zassert_equal(deserialized_logging_configuration.max_log_size_mb, logging_configuration.max_log_size_mb);

    zassert_equal(deserialized_logging_configuration.sensor_configurations.size(), logging_configuration.sensor_configurations.size());

    for(const auto& [sensor_id, sensor_configuration] : logging_configuration.sensor_configurations) {
        zassert_true(deserialized_logging_configuration.sensor_configurations.contains(sensor_id));

        zassert_equal(deserialized_logging_configuration.sensor_configurations[sensor_id].is_enabled, sensor_configuration.is_enabled);
        zassert_equal(deserialized_logging_configuration.sensor_configurations[sensor_id].log_raw_value, sensor_configuration.log_raw_value);
        zassert_equal(deserialized_logging_configuration.sensor_configurations[sensor_id].log_only_new_data, sensor_configuration.log_only_new_data);
    }
}

ZTEST(logging_configuration_parser, test_CborSerializeDeserialize) {
    LoggingConfigurationCborParser logging_configuration_cbor_parser;

    auto logging_configuration = logging_configuration_parser_GetTestConfiguration();

    auto serialized_logging_configuration = logging_configuration_cbor_parser.Serialize(logging_configuration);
    auto deserialized_logging_configuration = logging_configuration_cbor_parser.Deserialize(*serialized_logging_configuration.get());

    logging_configuration_parser_CompareLoggingConfigurations(logging_configuration, deserialized_logging_configuration);
}

ZTEST(logging_configuration_parser, test_JsonSerializeDeserialize) {
    LoggingConfigurationJsonParser logging_configuration_json_parser;

    auto logging_configuration = logging_configuration_parser_GetTestConfiguration();

    auto serialized_logging_configuration = logging_configuration_json_parser.Serialize(logging_configuration);
    auto deserialized_logging_configuration = logging_configuration_json_parser.Deserialize(*serialized_logging_configuration.get());

    logging_configuration_parser_CompareLoggingConfigurations(logging_configuration, deserialized_logging_configuration);
}
