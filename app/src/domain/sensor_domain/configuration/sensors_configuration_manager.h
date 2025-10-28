#pragma once

#include <vector>
#include <memory>
#include <span>

#include "utilities/math_parser/math_parser_service.hpp"
#include <utilities/memory/heap_allocator.h>
#include "configuration/sensor_config/sensor_config.h"
#include "configuration/services/configuration_service.h"
#include "domain/sensor_domain/models/sensor.h"

namespace eerie_leap::domain::sensor_domain::configuration {

using namespace eerie_leap::utilities::math_parser;
using namespace eerie_leap::utilities::memory;
using namespace eerie_leap::domain::sensor_domain::models;
using namespace eerie_leap::configuration::services;

class SensorsConfigurationManager {
private:
    std::shared_ptr<MathParserService> math_parser_service_;
    ext_unique_ptr<ConfigurationService<SensorsConfig>> sensors_configuration_service_;
    ext_unique_ptr<ExtVector> sensors_config_raw_;
    ext_unique_ptr<SensorsConfig> sensors_config_;
    std::vector<std::shared_ptr<Sensor>> sensors_;
    int adc_channel_count_;

public:
    SensorsConfigurationManager(std::shared_ptr<MathParserService> math_parser_service, ext_unique_ptr<ConfigurationService<SensorsConfig>> sensors_configuration_service, int adc_channel_count);

    bool Update(const std::vector<std::shared_ptr<Sensor>>& sensors);
    const std::vector<std::shared_ptr<Sensor>>* Get(bool force_load = false);
    const std::span<uint8_t> GetRaw();
};

} // namespace eerie_leap::domain::sensor_domain::configuration
