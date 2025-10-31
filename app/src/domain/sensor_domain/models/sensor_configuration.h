#pragma once

#include <memory>
#include <optional>

#include "utilities/memory/heap_allocator.h"
#include "utilities/math_parser/expression_evaluator.h"
#include "utilities/voltage_interpolator/i_voltage_interpolator.h"
#include "domain/sensor_domain/models/sources/canbus_source.h"

#include "sensor_type.h"

namespace eerie_leap::domain::sensor_domain::models {

using namespace eerie_leap::utilities::memory;
using namespace eerie_leap::utilities::math_parser;
using namespace eerie_leap::utilities::voltage_interpolator;
using namespace eerie_leap::domain::sensor_domain::models::sources;

struct SensorConfiguration {
    SensorType type;

    std::optional<uint32_t> channel = std::nullopt;
    std::string connection_string;
    uint32_t sampling_rate_ms = 100;

    ext_unique_ptr<IVoltageInterpolator> voltage_interpolator = nullptr;
    ext_unique_ptr<ExpressionEvaluator> expression_evaluator = nullptr;

    // connection_string data source decomposition objects
    ext_unique_ptr<CanbusSource> canbus_source = nullptr;

    void UpdateConnectionString() {
        if(type == SensorType::CANBUS_RAW || type == SensorType::CANBUS_ANALOG || type == SensorType::CANBUS_INDICATOR)
            connection_string = canbus_source->ToConnectionString();
        else
            connection_string = "";
    }

    void UnwrapConnectionString() {
        if(type == SensorType::CANBUS_RAW || type == SensorType::CANBUS_ANALOG || type == SensorType::CANBUS_INDICATOR)
            canbus_source = make_unique_ext<CanbusSource>(CanbusSource::FromConnectionString(connection_string));
    }
};

} // namespace eerie_leap::domain::sensor_domain::models
