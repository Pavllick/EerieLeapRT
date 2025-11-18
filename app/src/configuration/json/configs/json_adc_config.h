#pragma once

#include <vector>
#include <boost/json.hpp>
#include <nameof.hpp>

namespace eerie_leap::configuration::json::configs {

namespace json = boost::json;

struct JsonAdcCalibrationDataConfig {
    float voltage;
    float value;
};

struct JsonAdcChannelConfig {
    std::string interpolation_method;
    std::vector<JsonAdcCalibrationDataConfig> calibration_table;
};

struct JsonAdcConfig {
    uint32_t samples;
    std::vector<JsonAdcChannelConfig> channel_configurations;
};

static void tag_invoke(json::value_from_tag, json::value& jv, JsonAdcCalibrationDataConfig const& config) {
    jv = {
        {NAMEOF_MEMBER(&JsonAdcCalibrationDataConfig::voltage), config.voltage},
        {NAMEOF_MEMBER(&JsonAdcCalibrationDataConfig::value), config.value}
    };
}

static JsonAdcCalibrationDataConfig tag_invoke(json::value_to_tag<JsonAdcCalibrationDataConfig>, json::value const& jv) {
    json::object const& obj = jv.as_object();
    return JsonAdcCalibrationDataConfig{
        json::value_to<float>(obj.at(NAMEOF_MEMBER(&JsonAdcCalibrationDataConfig::voltage).c_str())),
        json::value_to<float>(obj.at(NAMEOF_MEMBER(&JsonAdcCalibrationDataConfig::value).c_str()))
    };
}

static void tag_invoke(json::value_from_tag, json::value& jv, JsonAdcChannelConfig const& config) {
    jv = {
        {NAMEOF_MEMBER(&JsonAdcChannelConfig::interpolation_method), config.interpolation_method},
        {NAMEOF_MEMBER(&JsonAdcChannelConfig::calibration_table), json::value_from(config.calibration_table)}
    };
}

static JsonAdcChannelConfig tag_invoke(json::value_to_tag<JsonAdcChannelConfig>, json::value const& jv) {
    json::object const& obj = jv.as_object();
    return JsonAdcChannelConfig{
        json::value_to<std::string>(obj.at(NAMEOF_MEMBER(&JsonAdcChannelConfig::interpolation_method).c_str())),
        json::value_to<std::vector<JsonAdcCalibrationDataConfig>>(obj.at(NAMEOF_MEMBER(&JsonAdcChannelConfig::calibration_table).c_str()))
    };
}

static void tag_invoke(json::value_from_tag, json::value& jv, JsonAdcConfig const& config) {
    jv = {
        {NAMEOF_MEMBER(&JsonAdcConfig::samples), config.samples},
        {NAMEOF_MEMBER(&JsonAdcConfig::channel_configurations), json::value_from(config.channel_configurations)}
    };
}

static JsonAdcConfig tag_invoke(json::value_to_tag<JsonAdcConfig>, json::value const& jv) {
    json::object const& obj = jv.as_object();
    return JsonAdcConfig{
        json::value_to<uint32_t>(obj.at(NAMEOF_MEMBER(&JsonAdcConfig::samples).c_str())),
        json::value_to<std::vector<JsonAdcChannelConfig>>(obj.at(NAMEOF_MEMBER(&JsonAdcConfig::channel_configurations).c_str()))
    };
}

static std::string json_encode_JsonAdcConfig(const JsonAdcConfig& config) {
    return json::serialize(json::value_from(config));
}

static JsonAdcConfig json_decode_JsonAdcConfig(std::string_view json_str) {
    json::value jv = json::parse(json_str);

    return json::value_to<JsonAdcConfig>(jv);
}

} // namespace eerie_leap::configuration::json::configs
