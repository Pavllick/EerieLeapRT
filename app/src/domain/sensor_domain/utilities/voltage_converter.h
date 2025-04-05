#pragma once

#include "domain/sensor_domain/models/sensor_configuration.h"

namespace eerie_leap::domain::sensor_domain::utilities {

using namespace eerie_leap::domain::sensor_domain::models;

class VoltageConverter {
public:
    static double ConvertVoltageToRawValue(const SensorConfiguration& configuration, double voltage);
};

} // namespace eerie_leap::domain::sensor_domain::utilities