#pragma once

#include <vector>
#include <memory>

#include "utilities/math_parser/math_parser_service.hpp"
#include "configuration/sensor_config/sensor_config.h"
#include "configuration/services/configuration_service.hpp"
#include "domain/sensor_domain/models/sensor.h"

namespace eerie_leap::controllers {

using namespace eerie_leap::utilities::math_parser;
using namespace eerie_leap::domain::sensor_domain::models;
using namespace eerie_leap::configuration::services;

class SensorsConfigurationController {
private:
    std::shared_ptr<MathParserService> math_parser_service_;
    std::shared_ptr<ConfigurationService<SensorsConfig>> sensors_configuration_service_;
    std::shared_ptr<SensorsConfig> sensors_config_;
    std::shared_ptr<std::vector<std::shared_ptr<Sensor>>> ordered_sensors_;

public:
    explicit SensorsConfigurationController(std::shared_ptr<ConfigurationService<SensorsConfig>> sensors_configuration_service) :
        sensors_configuration_service_(std::move(sensors_configuration_service)),
        sensors_config_(nullptr),
        ordered_sensors_(nullptr) {}

    void Initialize();
    bool Update(const std::shared_ptr<std::vector<std::shared_ptr<Sensor>>> sensors);
    const std::shared_ptr<std::vector<std::shared_ptr<Sensor>>> Get();
};

} // namespace eerie_leap::domain::sensor_domain::controllers