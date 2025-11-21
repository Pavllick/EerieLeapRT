#pragma once

#include <memory>
#include <span>

#include <zephyr/data/json.h>

#include "utilities/memory/heap_allocator.h"
#include "utilities/math_parser/math_parser_service.hpp"
#include "subsys/fs/services/i_fs_service.h"
#include "configuration/json/configs/json_sensors_config.h"
#include "domain/sensor_domain/models/sensor.h"

namespace eerie_leap::domain::sensor_domain::configuration::utilities::parsers {

using namespace eerie_leap::utilities::memory;
using namespace eerie_leap::utilities::math_parser;
using namespace eerie_leap::subsys::fs::services;
using namespace eerie_leap::configuration::json::configs;
using namespace eerie_leap::domain::sensor_domain::models;

class SensorsJsonParser {
private:
    std::shared_ptr<MathParserService> math_parser_service_;
    std::shared_ptr<IFsService> fs_service_;

public:
    explicit SensorsJsonParser(
        std::shared_ptr<MathParserService> math_parser_service,
        std::shared_ptr<IFsService> fs_service);

    ext_unique_ptr<JsonSensorsConfig> Serialize(
        const std::vector<std::shared_ptr<Sensor>>& sensors,
        uint32_t gpio_channel_count,
        uint32_t adc_channel_count);
    std::vector<std::shared_ptr<Sensor>> Deserialize(
        const JsonSensorsConfig& config,
        uint32_t gpio_channel_count,
        uint32_t adc_channel_count);
};

} // namespace eerie_leap::domain::sensor_domain::configuration::utilities::parsers
