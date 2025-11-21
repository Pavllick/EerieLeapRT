#pragma once

#include <vector>
#include <string>
#include <boost/json.hpp>
#include <nameof.hpp>

namespace eerie_leap::configuration::json::configs {

namespace json = boost::json;

struct JsonSensorMetadataConfig {
    std::string name;
    std::string unit;
    std::string description;
};

struct JsonSensorCalibrationDataConfig {
    float voltage;
    float value;
};

struct JsonSensorConfigurationConfig {
    std::string type;
    int32_t channel;
    std::string connection_string;
    std::string script_path;
    uint32_t sampling_rate_ms;
    std::string interpolation_method;
    std::vector<JsonSensorCalibrationDataConfig> calibration_table;
    std::string expression;
};

struct JsonSensorConfig {
    std::string id;
    JsonSensorMetadataConfig metadata;
    JsonSensorConfigurationConfig configuration;
};

struct JsonSensorsConfig {
    std::vector<JsonSensorConfig> sensors;
};

static void tag_invoke(json::value_from_tag, json::value& jv, JsonSensorMetadataConfig const& config) {
    jv = {
        {NAMEOF_MEMBER(&JsonSensorMetadataConfig::name), config.name},
        {NAMEOF_MEMBER(&JsonSensorMetadataConfig::unit), config.unit},
        {NAMEOF_MEMBER(&JsonSensorMetadataConfig::description), config.description}
    };
}

static JsonSensorMetadataConfig tag_invoke(json::value_to_tag<JsonSensorMetadataConfig>, json::value const& jv) {
    json::object const& obj = jv.as_object();
    return JsonSensorMetadataConfig{
        .name = json::value_to<std::string>(obj.at(NAMEOF_MEMBER(&JsonSensorMetadataConfig::name).c_str())),
        .unit = json::value_to<std::string>(obj.at(NAMEOF_MEMBER(&JsonSensorMetadataConfig::unit).c_str())),
        .description = json::value_to<std::string>(obj.at(NAMEOF_MEMBER(&JsonSensorMetadataConfig::description).c_str()))
    };
}

static void tag_invoke(json::value_from_tag, json::value& jv, JsonSensorCalibrationDataConfig const& config) {
    jv = {
        {NAMEOF_MEMBER(&JsonSensorCalibrationDataConfig::voltage), config.voltage},
        {NAMEOF_MEMBER(&JsonSensorCalibrationDataConfig::value), config.value}
    };
}

static JsonSensorCalibrationDataConfig tag_invoke(json::value_to_tag<JsonSensorCalibrationDataConfig>, json::value const& jv) {
    json::object const& obj = jv.as_object();
    return JsonSensorCalibrationDataConfig{
        .voltage = json::value_to<float>(obj.at(NAMEOF_MEMBER(&JsonSensorCalibrationDataConfig::voltage).c_str())),
        .value = json::value_to<float>(obj.at(NAMEOF_MEMBER(&JsonSensorCalibrationDataConfig::value).c_str()))
    };
}

static void tag_invoke(json::value_from_tag, json::value& jv, JsonSensorConfigurationConfig const& config) {
    jv = {
        {NAMEOF_MEMBER(&JsonSensorConfigurationConfig::type), config.type},
        {NAMEOF_MEMBER(&JsonSensorConfigurationConfig::channel), config.channel},
        {NAMEOF_MEMBER(&JsonSensorConfigurationConfig::connection_string), config.connection_string},
        {NAMEOF_MEMBER(&JsonSensorConfigurationConfig::script_path), config.script_path},
        {NAMEOF_MEMBER(&JsonSensorConfigurationConfig::sampling_rate_ms), config.sampling_rate_ms},
        {NAMEOF_MEMBER(&JsonSensorConfigurationConfig::interpolation_method), config.interpolation_method},
        {NAMEOF_MEMBER(&JsonSensorConfigurationConfig::calibration_table), json::value_from(config.calibration_table)},
        {NAMEOF_MEMBER(&JsonSensorConfigurationConfig::expression), config.expression}
    };
}

static JsonSensorConfigurationConfig tag_invoke(json::value_to_tag<JsonSensorConfigurationConfig>, json::value const& jv) {
    json::object const& obj = jv.as_object();
    return JsonSensorConfigurationConfig{
        .type = json::value_to<std::string>(obj.at(NAMEOF_MEMBER(&JsonSensorConfigurationConfig::type).c_str())),
        .channel = json::value_to<int32_t>(obj.at(NAMEOF_MEMBER(&JsonSensorConfigurationConfig::channel).c_str())),
        .connection_string = json::value_to<std::string>(obj.at(NAMEOF_MEMBER(&JsonSensorConfigurationConfig::connection_string).c_str())),
        .script_path = json::value_to<std::string>(obj.at(NAMEOF_MEMBER(&JsonSensorConfigurationConfig::script_path).c_str())),
        .sampling_rate_ms = json::value_to<uint32_t>(obj.at(NAMEOF_MEMBER(&JsonSensorConfigurationConfig::sampling_rate_ms).c_str())),
        .interpolation_method = json::value_to<std::string>(obj.at(NAMEOF_MEMBER(&JsonSensorConfigurationConfig::interpolation_method).c_str())),
        .calibration_table = json::value_to<std::vector<JsonSensorCalibrationDataConfig>>(obj.at(NAMEOF_MEMBER(&JsonSensorConfigurationConfig::calibration_table).c_str())),
        .expression = json::value_to<std::string>(obj.at(NAMEOF_MEMBER(&JsonSensorConfigurationConfig::expression).c_str()))
    };
}

static void tag_invoke(json::value_from_tag, json::value& jv, JsonSensorConfig const& config) {
    jv = {
        {NAMEOF_MEMBER(&JsonSensorConfig::id), config.id},
        {NAMEOF_MEMBER(&JsonSensorConfig::metadata), json::value_from(config.metadata)},
        {NAMEOF_MEMBER(&JsonSensorConfig::configuration), json::value_from(config.configuration)}
    };
}

static JsonSensorConfig tag_invoke(json::value_to_tag<JsonSensorConfig>, json::value const& jv) {
    json::object const& obj = jv.as_object();
    return JsonSensorConfig{
        .id = json::value_to<std::string>(obj.at(NAMEOF_MEMBER(&JsonSensorConfig::id).c_str())),
        .metadata = json::value_to<JsonSensorMetadataConfig>(obj.at(NAMEOF_MEMBER(&JsonSensorConfig::metadata).c_str())),
        .configuration = json::value_to<JsonSensorConfigurationConfig>(obj.at(NAMEOF_MEMBER(&JsonSensorConfig::configuration).c_str()))
    };
}

static void tag_invoke(json::value_from_tag, json::value& jv, JsonSensorsConfig const& config) {
    jv = {
        {NAMEOF_MEMBER(&JsonSensorsConfig::sensors), json::value_from(config.sensors)}
    };
}

static JsonSensorsConfig tag_invoke(json::value_to_tag<JsonSensorsConfig>, json::value const& jv) {
    json::object const& obj = jv.as_object();
    return JsonSensorsConfig{
       .sensors = json::value_to<std::vector<JsonSensorConfig>>(obj.at(NAMEOF_MEMBER(&JsonSensorsConfig::sensors).c_str()))
    };
}

static std::string json_encode_JsonSensorsConfig(const JsonSensorsConfig& config) {
    return json::serialize(json::value_from(config));
}

static JsonSensorsConfig json_decode_JsonSensorsConfig(std::string_view json_str) {
    json::value jv = json::parse(json_str);

    return json::value_to<JsonSensorsConfig>(jv);
}

} // namespace eerie_leap::configuration::json::configs
