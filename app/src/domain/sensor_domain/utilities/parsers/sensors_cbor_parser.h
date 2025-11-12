#pragma once

#include <vector>
#include <memory>
#include <span>

#include "utilities/math_parser/math_parser_service.hpp"
#include "utilities/memory/heap_allocator.h"
#include "configuration/cbor/cbor_sensor_config/cbor_sensor_config.h"
#include "domain/sensor_domain/models/sensor.h"

namespace eerie_leap::domain::sensor_domain::utilities::parsers {

using namespace eerie_leap::utilities::math_parser;
using namespace eerie_leap::utilities::memory;
using namespace eerie_leap::domain::sensor_domain::models;

class SensorsCborParser {
private:
    std::shared_ptr<MathParserService> math_parser_service_;

public:
    explicit SensorsCborParser(std::shared_ptr<MathParserService> math_parser_service);

    ext_unique_ptr<CborSensorsConfig> Serialize(
        const std::vector<std::shared_ptr<Sensor>>& sensors,
        uint32_t gpio_channel_count,
        uint32_t adc_channel_count);
    std::vector<std::shared_ptr<Sensor>> Deserialize(
        const CborSensorsConfig& sensors_config,
        uint32_t gpio_channel_count,
        uint32_t adc_channel_count);
};

} // namespace eerie_leap::domain::sensor_domain::utilities::parsers
