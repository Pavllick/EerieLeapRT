#include "sensors_configuration_service.h"

namespace eerie_leap::domain::sensor_domain::services {

using namespace eerie_leap::domain::sensor_domain::models;

void SensorsConfigurationService::UpdateSensors(const std::vector<Sensor>& sensors) {

}

const std::vector<Sensor>& SensorsConfigurationService::GetSensors() const {
    return ordered_sensors_;
}

} // namespace eerie_leap::domain::sensor_domain::services