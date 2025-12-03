#include "sensors_order_resolver.h"

namespace eerie_leap::domain::sensor_domain::utilities {

using namespace eerie_leap::domain::sensor_domain::models;

void SensorsOrderResolver::AddSensor(std::shared_ptr<Sensor> sensor) {
    if(sensor->configuration.expression_evaluator != nullptr) {
        auto sensor_ids = sensor->configuration.expression_evaluator->GetVariableNames();
        sensor_ids.erase("x");

        dependencies_.emplace(sensor->id, std::unordered_set<std::string>(sensor_ids.begin(), sensor_ids.end()));
    } else {
        dependencies_.emplace(sensor->id, std::unordered_set<std::string>());
    }

    sensors_.emplace(sensor->id, std::move(sensor));
}

bool SensorsOrderResolver::HasCyclicDependency(
    const std::string& sensor_id,
    std::unordered_set<std::string>& visited,
    std::unordered_set<std::string>& temp) {

    if(temp.contains(sensor_id))
        return true;
    if(visited.contains(sensor_id))
        return false;

    temp.insert(sensor_id);

    for(const auto& dep : dependencies_.at(sensor_id)) {
        if(!sensors_.contains(dep))
            throw std::runtime_error("Sensor "
                + sensor_id
                + " depends on non-existent sensor "
                + dep
                + ".");

        if(HasCyclicDependency(dep, visited, temp))
            return true;
    }

    temp.erase(sensor_id);
    visited.insert(sensor_id);

    return false;
}

void SensorsOrderResolver::ResolveDependencies(
    const std::string& sensor_id,
    std::unordered_set<std::string>& visited,
    std::vector<std::shared_ptr<Sensor>>& ordered_sensors) {

    if(visited.contains(sensor_id))
        return;

    visited.insert(sensor_id);

    for(const auto& dep : dependencies_.at(sensor_id))
        ResolveDependencies(dep, visited, ordered_sensors);

    ordered_sensors.push_back(sensors_.at(sensor_id));
}

std::vector<std::shared_ptr<Sensor>> SensorsOrderResolver::GetProcessingOrder() {
    std::unordered_set<std::string> visited;
    std::unordered_set<std::string> temp;

    for(const auto& [sensor_id, _] : sensors_) {
        if(!visited.contains(sensor_id)) {
            if(HasCyclicDependency(sensor_id, visited, temp))
                throw std::runtime_error("Cyclic dependency detected in sensor "
                    + sensor_id
                    + ".");
        }
    }

    visited.clear();
    std::vector<std::shared_ptr<Sensor>> ordered_sensors;

    for(const auto& [sensor_id, _] : sensors_)
        ResolveDependencies(sensor_id, visited, ordered_sensors);

    return ordered_sensors;
}

} // namespace eerie_leap::domain::sensor_domain::utilities
