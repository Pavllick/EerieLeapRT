#pragma once

#include <memory>
#include <vector>

#include "utilities/math_parser/math_parser_service.hpp"
#include "domain/sensor_domain/models/sensor.h"

namespace eerie_leap::domain::sensor_domain::services {

using namespace eerie_leap::utilities::math_parser;
using namespace eerie_leap::domain::sensor_domain::models;

class SensorsConfigurationService {
private:
    std::shared_ptr<MathParserService> math_parser_service_;
    std::vector<Sensor> ordered_sensors_;

public:
    void Initialize();
    void UpdateSensors(const std::vector<Sensor>& sensors);
    const std::vector<Sensor>& GetSensors() const;
};

} // namespace eerie_leap::domain::sensor_domain::services