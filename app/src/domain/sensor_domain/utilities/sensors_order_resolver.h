#pragma once

#include <memory>
#include <vector>
#include <unordered_map>
#include <unordered_set>

#include "domain/sensor_domain/models/sensor.h"

namespace eerie_leap::domain::sensor_domain::utilities {

using namespace eerie_leap::domain::sensor_domain::models;

class SensorsOrderResolver {
private:
    std::unordered_map<std::string, std::unordered_set<std::string>> dependencies_;
    std::unordered_map<std::string, std::shared_ptr<Sensor>> sensors_;

    bool HasCyclicDependency(
        const std::string& sensor_id,
        std::unordered_set<std::string>& visited,
        std::unordered_set<std::string>& temp);

    void ResolveDependencies(
        const std::string& sensor_id,
        std::unordered_set<std::string>& visited,
        std::vector<std::shared_ptr<Sensor>>& ordered_sensors);

public:
    void AddSensor(std::shared_ptr<Sensor> sensor);
    std::vector<std::shared_ptr<Sensor>> GetProcessingOrder();
};

} // namespace eerie_leap::domain::sensor_domain::utilities
