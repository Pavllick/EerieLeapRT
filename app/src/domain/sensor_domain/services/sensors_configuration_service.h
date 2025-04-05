#pragma once

#include <vector>

#include "domain/sensor_domain/models/sensor.h"

namespace eerie_leap::domain::sensor_domain::services {

using namespace eerie_leap::domain::sensor_domain::models;

class SensorsConfigurationService {
private:
    std::vector<Sensor> ordered_sensors_;

public:
    SensorsConfigurationService() = default;
    void UpdateSensors(const std::vector<Sensor>& sensors);
    const std::vector<Sensor>& GetSensors() const;
};

} // namespace eerie_leap::domain::sensor_domain::services