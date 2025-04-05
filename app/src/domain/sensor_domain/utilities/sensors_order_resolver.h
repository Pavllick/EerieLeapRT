#pragma once

#include <vector>
#include <unordered_map>
#include <unordered_set>
#include "domain/sensor_domain/models/sensor.h"

namespace eerie_leap::domain::sensor_domain::utilities {

using namespace eerie_leap::domain::sensor_domain::models;

class SensorsOrderResolver {
private:
    std::unordered_map<std::string, std::unordered_set<std::string>> dependencies_;
    std::unordered_map<std::string, Sensor> sensors_;

    bool HasCyclicDependency(
        const std::string& sensorId,
        std::unordered_set<std::string>& visited,
        std::unordered_set<std::string>& temp);

    void ResolveDependencies(
        const std::string& sensorId,
        std::unordered_set<std::string>& visited,
        std::vector<Sensor>& ordered_sensors);
    
public:
    void AddSensor(const Sensor& sensor);
    std::vector<Sensor> GetProcessingOrder();
};

} // namespace eerie_leap::domain::sensor_domain::utilities