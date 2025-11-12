#include "system_configuration_json_parser.h"

namespace eerie_leap::domain::system_domain::utilities::parsers {

ext_unique_ptr<ExtVector> SystemConfigurationJsonParser::Serialize(const SystemConfiguration& system_configuration) {
    SystemConfigurationJsonDto system_configuration_json;
    memset(&system_configuration_json, 0, sizeof(SystemConfigurationJsonDto));

    system_configuration_json.com_user_refresh_rate_ms = system_configuration.com_user_refresh_rate_ms;

    system_configuration_json.com_user_configurations_len = 0;

    for(size_t i = 0; i < system_configuration.com_user_configurations.size() && i < 8; i++) {
        system_configuration_json.com_user_configurations[i].device_id = system_configuration.com_user_configurations[i].device_id;
        system_configuration_json.com_user_configurations[i].server_id = system_configuration.com_user_configurations[i].server_id;

        system_configuration_json.com_user_configurations_len++;
    }

    system_configuration_json.canbus_configurations_len = 0;

    for(size_t i = 0; i < system_configuration.canbus_configurations.size() && i < 8; i++) {
        system_configuration_json.canbus_configurations[i].bus_channel = system_configuration.canbus_configurations[i].bus_channel;
        system_configuration_json.canbus_configurations[i].bitrate = system_configuration.canbus_configurations[i].bitrate;

        system_configuration_json.canbus_configurations_len++;
    }

    auto buf_size = json_calc_encoded_len(system_configuration_descr, ARRAY_SIZE(system_configuration_descr), &system_configuration_json);
    if(buf_size < 0)
        throw std::runtime_error("Failed to calculate buffer size.");

    // Add 1 for null terminator
    auto buffer = make_unique_ext<ExtVector>(buf_size + 1);

    int res = json_obj_encode_buf(system_configuration_descr, ARRAY_SIZE(system_configuration_descr), &system_configuration_json, (char*)buffer->data(), buffer->size());
    if(res != 0)
        throw std::runtime_error("Failed to serialize system configuration.");

    return buffer;
}

SystemConfiguration SystemConfigurationJsonParser::Deserialize(
    const std::span<const uint8_t>& json,
    uint64_t device_id,
    uint32_t hw_version,
    uint32_t sw_version,
    uint32_t build_number) {

	SystemConfigurationJsonDto system_configuration_json;
	const int expected_return_code = BIT_MASK(ARRAY_SIZE(system_configuration_descr));

	int ret = json_obj_parse((char*)json.data(), json.size(), system_configuration_descr, ARRAY_SIZE(system_configuration_descr), &system_configuration_json);
	if(ret != expected_return_code)
        throw std::runtime_error("Invalid JSON payload.");

    SystemConfiguration system_configuration;

    system_configuration.device_id = device_id;
    system_configuration.hw_version = hw_version;
    system_configuration.sw_version = sw_version;
    system_configuration.build_number = build_number;
    system_configuration.com_user_refresh_rate_ms = system_configuration_json.com_user_refresh_rate_ms;

    for(size_t i = 0; i < system_configuration_json.com_user_configurations_len; ++i) {
        const auto& com_user_config = system_configuration_json.com_user_configurations[i];
        system_configuration.com_user_configurations.push_back({
            .device_id = com_user_config.device_id,
            .server_id = static_cast<uint16_t>(com_user_config.server_id)
        });
    }

    for(size_t i = 0; i < system_configuration_json.canbus_configurations_len; ++i) {
        const auto& canbus_config = system_configuration_json.canbus_configurations[i];
        system_configuration.canbus_configurations.push_back({
            .bus_channel = static_cast<uint8_t>(canbus_config.bus_channel),
            .bitrate = canbus_config.bitrate
        });
    }

    return system_configuration;
}

} // eerie_leap::domain::system_domain::utilities::parsers
