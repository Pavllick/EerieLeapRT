#include "logging_configuration_json_parser.h"

namespace eerie_leap::domain::logging_domain::utilities::parsers {

ext_unique_ptr<ExtVector> LoggingConfigurationJsonParser::Serialize(const LoggingConfiguration& logging_configuration) {
    LoggingConfigurationJsonDto logging_configuration_json;
    memset(&logging_configuration_json, 0, sizeof(LoggingConfigurationJsonDto));

    logging_configuration_json.logging_interval_ms = logging_configuration.logging_interval_ms;
    logging_configuration_json.max_log_size_mb = logging_configuration.max_log_size_mb;

    logging_configuration_json.sensor_configurations_len = 0;

    int i = 0;
    for(auto& [sensor_id, sensor_configuration] : logging_configuration.sensor_configurations) {
        SensorLoggingConfigurationJsonDto sensor_logging_configuration_json {
            .sensor_id_hash = sensor_id,
            .is_enabled = sensor_configuration.is_enabled,
            .log_raw_value = sensor_configuration.log_raw_value,
            .log_only_new_data = sensor_configuration.log_only_new_data
        };

        logging_configuration_json.sensor_configurations[i] = sensor_logging_configuration_json;
        logging_configuration_json.sensor_configurations_len++;
        i++;
    }

    auto buf_size = json_calc_encoded_len(logging_configuration_descr, ARRAY_SIZE(logging_configuration_descr), &logging_configuration_json);
    if(buf_size < 0)
        throw std::runtime_error("Failed to calculate buffer size.");

    // Add 1 for null terminator
    auto buffer = make_unique_ext<ExtVector>(buf_size + 1);

    int res = json_obj_encode_buf(logging_configuration_descr, ARRAY_SIZE(logging_configuration_descr), &logging_configuration_json, (char*)buffer->data(), buffer->size());
    if(res != 0)
        throw std::runtime_error("Failed to serialize logging configuration.");

    return buffer;
}

LoggingConfiguration LoggingConfigurationJsonParser::Deserialize(const std::span<const uint8_t>& json) {
	LoggingConfigurationJsonDto logging_configuration_json;
	const int expected_return_code = BIT_MASK(ARRAY_SIZE(logging_configuration_descr));

	int ret = json_obj_parse((char*)json.data(), json.size(), logging_configuration_descr, ARRAY_SIZE(logging_configuration_descr), &logging_configuration_json);
	if(ret != expected_return_code)
        throw std::runtime_error("Invalid JSON payload.");

    LoggingConfiguration logging_configuration {
        .logging_interval_ms = logging_configuration_json.logging_interval_ms,
        .max_log_size_mb = logging_configuration_json.max_log_size_mb
    };

    for(size_t i = 0; i < logging_configuration_json.sensor_configurations_len; i++) {
        SensorLoggingConfiguration sensor_logging_configuration {
            .is_enabled = logging_configuration_json.sensor_configurations[i].is_enabled,
            .log_raw_value = logging_configuration_json.sensor_configurations[i].log_raw_value,
            .log_only_new_data = logging_configuration_json.sensor_configurations[i].log_only_new_data
        };
        logging_configuration.sensor_configurations[logging_configuration_json.sensor_configurations[i].sensor_id_hash] = sensor_logging_configuration;
    }

    return logging_configuration;
}

} // eerie_leap::domain::logging_domain::utilities::parsers
