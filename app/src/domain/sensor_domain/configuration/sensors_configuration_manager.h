#pragma once

#include <vector>
#include <memory>
#include <span>

#include "utilities/math_parser/math_parser_service.hpp"
#include <utilities/memory/heap_allocator.h>
#include "configuration/sensor_config/sensor_config.h"
#include "configuration/services/cbor_configuration_service.h"
#include "domain/sensor_domain/utilities/parsers/sensors_cbor_parser.h"
#include "domain/sensor_domain/models/sensor.h"

namespace eerie_leap::domain::sensor_domain::configuration {

using namespace eerie_leap::utilities::math_parser;
using namespace eerie_leap::utilities::memory;
using namespace eerie_leap::configuration::services;
using namespace eerie_leap::domain::sensor_domain::models;
using namespace eerie_leap::domain::sensor_domain::utilities::parsers;

class SensorsConfigurationManager {
private:
    std::shared_ptr<MathParserService> math_parser_service_;
    ext_unique_ptr<CborConfigurationService<SensorsConfig>> cbor_configuration_service_;
    ext_unique_ptr<ExtVector> config_raw_;
    ext_unique_ptr<SensorsConfig> config_;
    std::vector<std::shared_ptr<Sensor>> sensors_;
    int gpio_channel_count_;
    int adc_channel_count_;
    std::unique_ptr<SensorsCborParser> sensors_cbor_parser_;

    bool CreateDefaultConfiguration();

public:
    SensorsConfigurationManager(
        std::shared_ptr<MathParserService> math_parser_service,
        ext_unique_ptr<CborConfigurationService<SensorsConfig>> cbor_configuration_service,
        int gpio_channel_count,
        int adc_channel_count);

    bool Update(const std::vector<std::shared_ptr<Sensor>>& sensors);
    const std::vector<std::shared_ptr<Sensor>>* Get(bool force_load = false);
    const std::span<uint8_t> GetRaw();
};

} // namespace eerie_leap::domain::sensor_domain::configuration
