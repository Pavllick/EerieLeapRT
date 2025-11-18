#pragma once

#include <vector>
#include <memory>
#include <span>

#include "utilities/math_parser/math_parser_service.hpp"
#include <utilities/memory/heap_allocator.h>
#include "configuration/cbor/cbor_sensors_config/cbor_sensors_config.h"
#include "configuration/services/cbor_configuration_service.h"
#include "configuration/json/configs/json_sensors_config.h"
#include "configuration/services/json_configuration_service.h"
#include "domain/sensor_domain/utilities/parsers/sensors_cbor_parser.h"
#include "domain/sensor_domain/utilities/parsers/sensors_json_parser.h"
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

    ext_unique_ptr<CborConfigurationService<CborSensorsConfig>> cbor_configuration_service_;
    ext_unique_ptr<JsonConfigurationService<JsonSensorsConfig>> json_configuration_service_;

    std::unique_ptr<SensorsCborParser> cbor_parser_;
    std::unique_ptr<SensorsJsonParser> json_parser_;

    std::vector<std::shared_ptr<Sensor>> sensors_;
    int gpio_channel_count_;
    int adc_channel_count_;

    uint32_t json_config_checksum_;

    bool ApplyJsonConfiguration();
    bool CreateDefaultConfiguration();

public:
    SensorsConfigurationManager(
        std::shared_ptr<MathParserService> math_parser_service,
        ext_unique_ptr<CborConfigurationService<CborSensorsConfig>> cbor_configuration_service,
        ext_unique_ptr<JsonConfigurationService<JsonSensorsConfig>> json_configuration_service,
        int gpio_channel_count,
        int adc_channel_count);

    bool Update(const std::vector<std::shared_ptr<Sensor>>& sensors);
    const std::vector<std::shared_ptr<Sensor>>* Get(bool force_load = false);
};

} // namespace eerie_leap::domain::sensor_domain::configuration
