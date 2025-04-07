#pragma once

#include <unordered_map>
#include <string>
#include <stdexcept>
#include <memory>

#include "domain/sensor_domain/models/sensor_reading.h"

namespace eerie_leap::domain::sensor_domain::utilities {

using namespace eerie_leap::domain::sensor_domain::models;

class SensorReadingsFrame {
private:
    std::unordered_map<std::string, std::shared_ptr<SensorReading>> readings_;

public:
    SensorReadingsFrame() = default;

    void AddOrUpdateReading(std::shared_ptr<SensorReading> sensor_reading) {
        readings_[sensor_reading->sensor.id] = std::move(sensor_reading);
    }

    std::shared_ptr<SensorReading> GetReading(const std::string& sensorId) const {
        auto it = readings_.find(sensorId);
        if (it != readings_.end())
            return it->second;

        throw std::runtime_error("Sensor ID not found");
    }

    const std::unordered_map<std::string, std::shared_ptr<SensorReading>>& GetReadings() const {
        return readings_;
    }

    const std::unordered_map<std::string, double> GetReadingsValues() const {
        std::unordered_map<std::string, double> readings;
        for (const auto& [id, reading] : readings_)
            readings[id] = reading->value.value();

        return readings;
    }

    void ClearReadings() {
        readings_.clear();
    }
};

} // eerie_leap::domain::sensor_domain::utilities