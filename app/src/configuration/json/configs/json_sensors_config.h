#pragma once

#include <boost/container/pmr/vector.hpp>
#include <boost/json.hpp>
#include <nameof.hpp>

#include "utilities/memory/memory_resource_manager.h"

namespace eerie_leap::configuration::json::configs {

namespace json = boost::json;
using namespace eerie_leap::utilities::memory;

struct JsonSensorMetadataConfig {
    json::string name;
    json::string unit;
    json::string description;

    JsonSensorMetadataConfig(json::storage_ptr sp = Mrm::GetBoostExtPmr())
        : name(sp), unit(sp), description(sp) {}
};

struct JsonSensorCalibrationDataConfig {
    float voltage;
    float value;
};

struct JsonSensorConfigurationConfig {
    json::string type;
    int32_t channel;
    json::string connection_string;
    json::string script_path;
    uint32_t sampling_rate_ms;
    json::string interpolation_method;
    boost::container::pmr::vector<JsonSensorCalibrationDataConfig> calibration_table;
    json::string expression;

    JsonSensorConfigurationConfig(json::storage_ptr sp = Mrm::GetBoostExtPmr())
        : type(sp), connection_string(sp), script_path(sp), interpolation_method(sp), calibration_table(sp.get()), expression(sp) {}
};

struct JsonSensorConfig {
    json::string id;
    JsonSensorMetadataConfig metadata;
    JsonSensorConfigurationConfig configuration;

    JsonSensorConfig(json::storage_ptr sp = Mrm::GetBoostExtPmr())
        : id(sp), metadata(sp), configuration(sp) {}
};

struct JsonSensorsConfig {
    boost::container::pmr::vector<JsonSensorConfig> sensors;

    JsonSensorsConfig(json::storage_ptr sp = Mrm::GetBoostExtPmr())
        : sensors(sp.get()) {}
};

static void tag_invoke(json::value_from_tag, json::value& jv, JsonSensorMetadataConfig const& config) {
    jv.~value();
    new(&jv) json::value(json::object(Mrm::GetBoostExtPmr()));
    json::object& obj = jv.as_object();

    obj[NAMEOF_MEMBER(&JsonSensorMetadataConfig::name).c_str()] = config.name;
    obj[NAMEOF_MEMBER(&JsonSensorMetadataConfig::unit).c_str()] = config.unit;
    obj[NAMEOF_MEMBER(&JsonSensorMetadataConfig::description).c_str()] = config.description;

    jv = std::move(obj);
}

static JsonSensorMetadataConfig tag_invoke(json::value_to_tag<JsonSensorMetadataConfig>, json::value const& jv) {
    json::object const& obj = jv.as_object();

    JsonSensorMetadataConfig result;
    result.name = obj.at(NAMEOF_MEMBER(&JsonSensorMetadataConfig::name).c_str()).as_string();
    result.unit = obj.at(NAMEOF_MEMBER(&JsonSensorMetadataConfig::unit).c_str()).as_string();
    result.description = obj.at(NAMEOF_MEMBER(&JsonSensorMetadataConfig::description).c_str()).as_string();

    return result;
}

static void tag_invoke(json::value_from_tag, json::value& jv, JsonSensorCalibrationDataConfig const& config) {
    jv.~value();
    new(&jv) json::value(json::object(Mrm::GetBoostExtPmr()));
    json::object& obj = jv.as_object();

    obj[NAMEOF_MEMBER(&JsonSensorCalibrationDataConfig::voltage).c_str()] = config.voltage;
    obj[NAMEOF_MEMBER(&JsonSensorCalibrationDataConfig::value).c_str()] = config.value;

    jv = std::move(obj);
}

static JsonSensorCalibrationDataConfig tag_invoke(json::value_to_tag<JsonSensorCalibrationDataConfig>, json::value const& jv) {
    json::object const& obj = jv.as_object();
    return {
        .voltage = static_cast<float>(obj.at(NAMEOF_MEMBER(&JsonSensorCalibrationDataConfig::voltage).c_str()).to_number<double>()),
        .value = static_cast<float>(obj.at(NAMEOF_MEMBER(&JsonSensorCalibrationDataConfig::value).c_str()).to_number<double>())
    };
}

static void tag_invoke(json::value_from_tag, json::value& jv, JsonSensorConfigurationConfig const& config) {
    jv.~value();
    new(&jv) json::value(json::object(Mrm::GetBoostExtPmr()));
    json::object& obj = jv.as_object();

    obj[NAMEOF_MEMBER(&JsonSensorConfigurationConfig::type).c_str()] = config.type;
    obj[NAMEOF_MEMBER(&JsonSensorConfigurationConfig::channel).c_str()] = config.channel;
    obj[NAMEOF_MEMBER(&JsonSensorConfigurationConfig::connection_string).c_str()] = config.connection_string;
    obj[NAMEOF_MEMBER(&JsonSensorConfigurationConfig::script_path).c_str()] = config.script_path;
    obj[NAMEOF_MEMBER(&JsonSensorConfigurationConfig::sampling_rate_ms).c_str()] = config.sampling_rate_ms;
    obj[NAMEOF_MEMBER(&JsonSensorConfigurationConfig::interpolation_method).c_str()] = config.interpolation_method;

    json::array calib_array(Mrm::GetBoostExtPmr());
    for(const auto& calib : config.calibration_table)
        calib_array.push_back(json::value_from(calib, Mrm::GetBoostExtPmr()));
    obj[NAMEOF_MEMBER(&JsonSensorConfigurationConfig::calibration_table).c_str()] = std::move(calib_array);

    obj[NAMEOF_MEMBER(&JsonSensorConfigurationConfig::expression).c_str()] = config.expression;

    jv = std::move(obj);
}

static JsonSensorConfigurationConfig tag_invoke(json::value_to_tag<JsonSensorConfigurationConfig>, json::value const& jv) {
    json::object const& obj = jv.as_object();
    JsonSensorConfigurationConfig result;

    result.type = obj.at(NAMEOF_MEMBER(&JsonSensorConfigurationConfig::type).c_str()).as_string();
    result.channel = static_cast<int32_t>(obj.at(NAMEOF_MEMBER(&JsonSensorConfigurationConfig::channel).c_str()).as_int64());
    result.connection_string = obj.at(NAMEOF_MEMBER(&JsonSensorConfigurationConfig::connection_string).c_str()).as_string();
    result.script_path = obj.at(NAMEOF_MEMBER(&JsonSensorConfigurationConfig::script_path).c_str()).as_string();
    result.sampling_rate_ms = static_cast<uint32_t>(obj.at(NAMEOF_MEMBER(&JsonSensorConfigurationConfig::sampling_rate_ms).c_str()).as_int64());
    result.interpolation_method = obj.at(NAMEOF_MEMBER(&JsonSensorConfigurationConfig::interpolation_method).c_str()).as_string();

    const json::array& calib_array = obj.at(NAMEOF_MEMBER(&JsonSensorConfigurationConfig::calibration_table).c_str()).as_array();
    result.calibration_table.reserve(calib_array.size());
    for(const auto& elem : calib_array)
        result.calibration_table.push_back(json::value_to<JsonSensorCalibrationDataConfig>(elem));

    result.expression = obj.at(NAMEOF_MEMBER(&JsonSensorConfigurationConfig::expression).c_str()).as_string();

    return result;
}

static void tag_invoke(json::value_from_tag, json::value& jv, JsonSensorConfig const& config) {
    jv.~value();
    new(&jv) json::value(json::object(Mrm::GetBoostExtPmr()));
    json::object& obj = jv.as_object();

    obj[NAMEOF_MEMBER(&JsonSensorConfig::id).c_str()] = config.id;
    obj[NAMEOF_MEMBER(&JsonSensorConfig::metadata).c_str()] = json::value_from(config.metadata, Mrm::GetBoostExtPmr());
    obj[NAMEOF_MEMBER(&JsonSensorConfig::configuration).c_str()] = json::value_from(config.configuration, Mrm::GetBoostExtPmr());

    jv = std::move(obj);
}

static JsonSensorConfig tag_invoke(json::value_to_tag<JsonSensorConfig>, json::value const& jv) {
    json::object const& obj = jv.as_object();
    JsonSensorConfig result;

    result.id = obj.at(NAMEOF_MEMBER(&JsonSensorConfig::id).c_str()).as_string();
    result.metadata = json::value_to<JsonSensorMetadataConfig>(obj.at(NAMEOF_MEMBER(&JsonSensorConfig::metadata).c_str()));
    result.configuration = json::value_to<JsonSensorConfigurationConfig>(obj.at(NAMEOF_MEMBER(&JsonSensorConfig::configuration).c_str()));

    return result;
}

static void tag_invoke(json::value_from_tag, json::value& jv, JsonSensorsConfig const& config) {
    jv.~value();
    new(&jv) json::value(json::object(Mrm::GetBoostExtPmr()));
    json::object& obj = jv.as_object();

    json::array sensors_array(Mrm::GetBoostExtPmr());
    for(const auto& sensor : config.sensors)
        sensors_array.push_back(json::value_from(sensor, Mrm::GetBoostExtPmr()));
    obj[NAMEOF_MEMBER(&JsonSensorsConfig::sensors).c_str()] = std::move(sensors_array);

    jv = std::move(obj);
}

static JsonSensorsConfig tag_invoke(json::value_to_tag<JsonSensorsConfig>, json::value const& jv) {
    json::object const& obj = jv.as_object();
    JsonSensorsConfig result;

    const json::array& sensors_array = obj.at(NAMEOF_MEMBER(&JsonSensorsConfig::sensors).c_str()).as_array();
    result.sensors.reserve(sensors_array.size());
    for(const auto& elem : sensors_array)
        result.sensors.push_back(json::value_to<JsonSensorConfig>(elem));

    return result;
}

} // namespace eerie_leap::configuration::json::configs
