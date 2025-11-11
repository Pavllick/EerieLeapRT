#pragma once

#include <vector>
#include <memory>
#include <span>

#include "utilities/math_parser/math_parser_service.hpp"
#include "utilities/memory/heap_allocator.h"
#include "configuration/sensor_config/sensor_config.h"
#include "domain/sensor_domain/models/sensor.h"

namespace eerie_leap::domain::sensor_domain::utilities::parsers {

using namespace eerie_leap::utilities::math_parser;
using namespace eerie_leap::utilities::memory;
using namespace eerie_leap::domain::sensor_domain::models;

class SensorsCborParser {
private:
    std::shared_ptr<MathParserService> math_parser_service_;

    void ValidateSensorType(const SensorConfiguration& sensor_configuration);

public:
    explicit SensorsCborParser(std::shared_ptr<MathParserService> math_parser_service);

    std::vector<std::shared_ptr<Sensor>> Deserialize(const SensorsConfig& sensors_config);
    ext_unique_ptr<SensorsConfig> Serialize(
        const std::vector<std::shared_ptr<Sensor>>& sensors,
        size_t gpio_channel_count,
        size_t adc_channel_count);
};

} // namespace eerie_leap::domain::sensor_domain::utilities::parsers
