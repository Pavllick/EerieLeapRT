#pragma once

#include <memory>
#include <vector>
#include <unordered_map>
#include <unordered_set>

#include "domain/sensor_domain/models/sensor.h"

namespace eerie_leap::domain::sensor_domain::utilities {

using namespace eerie_leap::domain::sensor_domain::models;

// NOTE: Cureently only used for dependency validation.
class SensorsOrderResolver {
private:
    std::unordered_map<size_t, std::unordered_set<size_t>> dependencies_;
    std::unordered_map<size_t, std::shared_ptr<Sensor>> sensors_;

    bool HasCyclicDependency(
        const size_t sensor_id,
        std::unordered_set<size_t>& visited,
        std::unordered_set<size_t>& temp);

    void ResolveDependencies(
        const size_t sensor_id,
        std::unordered_set<size_t>& visited,
        std::vector<std::shared_ptr<Sensor>>& ordered_sensors);

public:
    void AddSensor(std::shared_ptr<Sensor> sensor);
    std::vector<std::shared_ptr<Sensor>> GetProcessingOrder();
};

} // namespace eerie_leap::domain::sensor_domain::utilities
