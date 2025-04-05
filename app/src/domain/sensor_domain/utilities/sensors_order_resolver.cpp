#include <stdexcept>
#include <algorithm>

#include "sensors_order_resolver.h"
#include "utilities/expression_evaluator.h"
#include "domain/sensor_domain/models/sensor_type.h"

namespace eerie_leap::domain::sensor_domain::utilities {

using namespace eerie_leap::utilities;
using namespace eerie_leap::domain::sensor_domain::models;

void SensorsOrderResolver::AddSensor(const Sensor& sensor) {
    sensors_[sensor.id] = sensor;

    auto sensorIds = ExpressionEvaluator::ExtractSensorIds(sensor.configuration.conversion_expression_sanitized.value_or(""));
    dependencies_[sensor.id] = std::unordered_set<std::string>(sensorIds.begin(), sensorIds.end());
}

bool SensorsOrderResolver::HasCyclicDependency(
    const std::string& sensorId,
    std::unordered_set<std::string>& visited,
    std::unordered_set<std::string>& temp
) {
    if(temp.contains(sensorId))
        return true;
    if(visited.contains(sensorId))
        return false;

    temp.insert(sensorId);

    for(const auto& dep : dependencies_.at(sensorId)) {
        if(!sensors_.contains(dep))
            throw std::runtime_error("Sensor " + sensorId + " depends on non-existent sensor " + dep);

        if(HasCyclicDependency(dep, visited, temp))
            return true;
    }

    temp.erase(sensorId);
    visited.insert(sensorId);

    return false;
}

void SensorsOrderResolver::ResolveDependencies(
    const std::string& sensorId,
    std::unordered_set<std::string>& visited,
    std::vector<Sensor>& ordered_sensors
) {
    if(visited.contains(sensorId))
        return;

    visited.insert(sensorId);

    for(const auto& dep : dependencies_.at(sensorId)) {
        ResolveDependencies(dep, visited, ordered_sensors);
    }

    ordered_sensors.push_back(sensors_.at(sensorId));
}

std::vector<Sensor> SensorsOrderResolver::GetProcessingOrder() {
    std::unordered_set<std::string> visited;
    std::unordered_set<std::string> temp;

    for(const auto& [sensorId, _] : sensors_) {
        if(!visited.contains(sensorId))
            if(HasCyclicDependency(sensorId, visited, temp))
                throw std::runtime_error("Cyclic dependency detected in sensor " + sensorId);
    }

    visited.clear();
    std::vector<Sensor> ordered_sensors;

    for(const auto& [sensorId, _] : sensors_) {
        ResolveDependencies(sensorId, visited, ordered_sensors);
    }

    return ordered_sensors;
}

} // namespace eerie_leap::domain::sensor_domain::utilities