#pragma once

#include <memory>
#include <optional>

#include "utilities/math_parser/expression_evaluator.h"
#include "utilities/voltage_interpolator/i_voltage_interpolator.h"
#include "subsys/lua_script/lua_script.h"
#include "domain/sensor_domain/models/sources/canbus_source.h"

#include "sensor_type.h"

namespace eerie_leap::domain::sensor_domain::models {

using namespace eerie_leap::utilities::math_parser;
using namespace eerie_leap::utilities::voltage_interpolator;
using namespace eerie_leap::subsys::lua_script;
using namespace eerie_leap::domain::sensor_domain::models::sources;

struct SensorConfiguration {
    SensorType type;

    std::optional<uint32_t> channel = std::nullopt;
    std::string connection_string;
    std::string script_path;
    uint32_t sampling_rate_ms = 100;

    std::unique_ptr<IVoltageInterpolator> voltage_interpolator = nullptr;
    std::unique_ptr<ExpressionEvaluator> expression_evaluator = nullptr;
    std::shared_ptr<LuaScript> lua_script = nullptr;

    // connection_string data source decomposition objects
    std::unique_ptr<CanbusSource> canbus_source = nullptr;

    void UpdateConnectionString() {
        if(type == SensorType::CANBUS_RAW || type == SensorType::CANBUS_ANALOG || type == SensorType::CANBUS_INDICATOR)
            connection_string = canbus_source->ToConnectionString();
        else
            connection_string = "";
    }

    void UnwrapConnectionString() {
        if(type == SensorType::CANBUS_RAW || type == SensorType::CANBUS_ANALOG || type == SensorType::CANBUS_INDICATOR)
            canbus_source = std::make_unique<CanbusSource>(CanbusSource::FromConnectionString(connection_string));
    }
};

} // namespace eerie_leap::domain::sensor_domain::models
