#pragma once

#include <boost/container/pmr/vector.hpp>
#include <boost/json.hpp>
#include <nameof.hpp>

#include "utilities/memory/memory_resource_manager.h"

namespace eerie_leap::configuration::json::configs {

namespace json = boost::json;
using namespace eerie_leap::utilities::memory;

struct JsonAdcCalibrationDataConfig {
    float voltage;
    float value;
};

struct JsonAdcChannelConfig {
    json::string interpolation_method;
    boost::container::pmr::vector<JsonAdcCalibrationDataConfig> calibration_table;

    JsonAdcChannelConfig(json::storage_ptr sp = Mrm::GetBoostExtPmr())
        : interpolation_method(sp), calibration_table(sp.get()) {}
};

struct JsonAdcConfig {
    uint32_t samples;
    boost::container::pmr::vector<JsonAdcChannelConfig> channel_configs;

    JsonAdcConfig(json::storage_ptr sp = Mrm::GetBoostExtPmr())
        : channel_configs(sp.get()) {}
};

static void tag_invoke(json::value_from_tag, json::value& jv, JsonAdcCalibrationDataConfig const& config) {
    jv.~value();
    new(&jv) json::value(json::object(Mrm::GetBoostExtPmr()));
    json::object& obj = jv.as_object();

    obj[NAMEOF_MEMBER(&JsonAdcCalibrationDataConfig::voltage).c_str()] = config.voltage;
    obj[NAMEOF_MEMBER(&JsonAdcCalibrationDataConfig::value).c_str()] = config.value;

    jv = std::move(obj);
}

static JsonAdcCalibrationDataConfig tag_invoke(json::value_to_tag<JsonAdcCalibrationDataConfig>, json::value const& jv) {
    json::object const& obj = jv.as_object();
    return {
        .voltage = static_cast<float>(obj.at(NAMEOF_MEMBER(&JsonAdcCalibrationDataConfig::voltage).c_str()).to_number<double>()),
        .value = static_cast<float>(obj.at(NAMEOF_MEMBER(&JsonAdcCalibrationDataConfig::value).c_str()).to_number<double>())
    };
}

static void tag_invoke(json::value_from_tag, json::value& jv, JsonAdcChannelConfig const& config) {
    jv.~value();
    new(&jv) json::value(json::object(Mrm::GetBoostExtPmr()));
    json::object& obj = jv.as_object();

    obj[NAMEOF_MEMBER(&JsonAdcChannelConfig::interpolation_method).c_str()] = config.interpolation_method;

    json::array calibration_table_array(Mrm::GetBoostExtPmr());
    for(const auto& elem : config.calibration_table)
        calibration_table_array.push_back(json::value_from(elem, Mrm::GetBoostExtPmr()));
    obj[NAMEOF_MEMBER(&JsonAdcChannelConfig::calibration_table).c_str()] = std::move(calibration_table_array);

    jv = std::move(obj);
}

static JsonAdcChannelConfig tag_invoke(json::value_to_tag<JsonAdcChannelConfig>, json::value const& jv) {
    json::object const& obj = jv.as_object();
    JsonAdcChannelConfig result;

    result.interpolation_method = obj.at(NAMEOF_MEMBER(&JsonAdcChannelConfig::interpolation_method).c_str()).as_string();

    const json::array& calibration_table_array = obj.at(NAMEOF_MEMBER(&JsonAdcChannelConfig::calibration_table).c_str()).as_array();
    result.calibration_table.reserve(calibration_table_array.size());
    for(const auto& elem : calibration_table_array)
        result.calibration_table.push_back(json::value_to<JsonAdcCalibrationDataConfig>(elem));

    return result;
}

static void tag_invoke(json::value_from_tag, json::value& jv, JsonAdcConfig const& config) {
    jv.~value();
    new(&jv) json::value(json::object(Mrm::GetBoostExtPmr()));
    json::object& obj = jv.as_object();

    obj[NAMEOF_MEMBER(&JsonAdcConfig::samples).c_str()] = config.samples;

    json::array channel_configs_array(Mrm::GetBoostExtPmr());
    for(const auto& channel_config : config.channel_configs)
        channel_configs_array.push_back(json::value_from(channel_config, Mrm::GetBoostExtPmr()));
    obj[NAMEOF_MEMBER(&JsonAdcConfig::channel_configs).c_str()] = std::move(channel_configs_array);

    jv = std::move(obj);
}

static JsonAdcConfig tag_invoke(json::value_to_tag<JsonAdcConfig>, json::value const& jv) {
    json::object const& obj = jv.as_object();
    JsonAdcConfig result;

    result.samples = static_cast<int32_t>(obj.at(NAMEOF_MEMBER(&JsonAdcConfig::samples).c_str()).as_int64());

    const json::array& channel_configs_array = obj.at(NAMEOF_MEMBER(&JsonAdcConfig::channel_configs).c_str()).as_array();
    result.channel_configs.reserve(channel_configs_array.size());
    for(const auto& elem : channel_configs_array)
        result.channel_configs.push_back(json::value_to<JsonAdcChannelConfig>(elem));

    return result;
}

} // namespace eerie_leap::configuration::json::configs
