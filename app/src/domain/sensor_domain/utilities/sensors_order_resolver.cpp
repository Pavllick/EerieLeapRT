#include "sensors_order_resolver.h"

namespace eerie_leap::domain::sensor_domain::utilities {

using namespace eerie_leap::domain::sensor_domain::models;

void SensorsOrderResolver::AddSensor(std::shared_ptr<Sensor> sensor) {
    size_t sensor_id_hash = sensor->id_hash;
    sensors_.emplace(sensor_id_hash, sensor);

    if(sensor->configuration.expression_evaluator != nullptr) {
        auto sensor_ids = sensor->configuration.expression_evaluator->GetVariableNameHashes();
        dependencies_.emplace(sensor_id_hash, std::unordered_set<size_t>(sensor_ids.begin(), sensor_ids.end()));
    } else {
        dependencies_.emplace(sensor_id_hash, std::unordered_set<size_t>());
    }
}

bool SensorsOrderResolver::HasCyclicDependency(
    const size_t sensor_id_hash,
    std::unordered_set<size_t>& visited,
    std::unordered_set<size_t>& temp) {

    if(temp.contains(sensor_id_hash))
        return true;
    if(visited.contains(sensor_id_hash))
        return false;

    temp.insert(sensor_id_hash);

    for(const auto& dep : dependencies_.at(sensor_id_hash)) {
        if(!sensors_.contains(dep))
            throw std::runtime_error("Sensor "
                + sensors_.at(sensor_id_hash)->id
                + " depends on non-existent sensor "
                + sensors_.at(dep)->configuration.expression_evaluator->GetVariableName(dep)
                + ".");

        if(HasCyclicDependency(dep, visited, temp))
            return true;
    }

    temp.erase(sensor_id_hash);
    visited.insert(sensor_id_hash);

    return false;
}

void SensorsOrderResolver::ResolveDependencies(
    const size_t sensor_id_hash,
    std::unordered_set<size_t>& visited,
    std::vector<std::shared_ptr<Sensor>>& ordered_sensors
) {
    if(visited.contains(sensor_id_hash))
        return;

    visited.insert(sensor_id_hash);

    for(const auto& dep : dependencies_.at(sensor_id_hash)) {
        ResolveDependencies(dep, visited, ordered_sensors);
    }

    ordered_sensors.push_back(sensors_.at(sensor_id_hash));
}

std::vector<std::shared_ptr<Sensor>> SensorsOrderResolver::GetProcessingOrder() {
    std::unordered_set<size_t> visited;
    std::unordered_set<size_t> temp;

    for(const auto& [sensor_id_hash, _] : sensors_) {
        if(!visited.contains(sensor_id_hash))
            if(HasCyclicDependency(sensor_id_hash, visited, temp))
                throw std::runtime_error("Cyclic dependency detected in sensor "
                    + sensors_.at(sensor_id_hash)->id
                    + ".");
    }

    visited.clear();
    std::vector<std::shared_ptr<Sensor>> ordered_sensors;

    for(const auto& [sensor_id_hash, _] : sensors_) {
        ResolveDependencies(sensor_id_hash, visited, ordered_sensors);
    }

    return ordered_sensors;
}

} // namespace eerie_leap::domain::sensor_domain::utilities
