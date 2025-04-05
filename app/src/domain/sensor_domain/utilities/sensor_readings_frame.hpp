#pragma once

#include <unordered_map>
#include <string>
#include <stdexcept>

namespace eerie_leap::domain::sensor_domain::utilities
{

using namespace eerie_leap::domain::sensor_domain::models;

class SensorReadingsFrame {
private:
    std::unordered_map<std::string, double> readings_;

public:
    SensorReadingsFrame() = default;

    void AddReading(const std::string& sensorId, double value) {
        readings_[sensorId] = value;
    }

    double GetReading(const std::string& sensorId) const {
        auto it = readings_.find(sensorId);
        if (it != readings_.end())
            return it->second;

        throw std::runtime_error("Sensor ID not found");
    }

    const std::unordered_map<std::string, double>& GetAllReadings() const {
        return readings_;
    }

    void Clear() {
        readings_.clear();
    }
};

} // eerie_leap::domain::sensor_domain::utilities