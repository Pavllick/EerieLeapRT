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
    std::unordered_map<std::string, float*> readings_values_;

public:
    SensorReadingsFrame() = default;

    void AddOrUpdateReading(std::shared_ptr<SensorReading> reading) {
        std::string sensor_id = reading->sensor->id;

        readings_[sensor_id] = reading;

        if (readings_[sensor_id]->value.has_value() && readings_[sensor_id]->status != ReadingStatus::ERROR)
            readings_values_[sensor_id] = &readings_[sensor_id]->value.value();
    }

    std::shared_ptr<SensorReading> GetReading(const std::string& sensor_id) const {
        auto it = readings_.find(sensor_id);
        if (it != readings_.end())
            return it->second;

        throw std::runtime_error("Sensor ID not found");
    }

    const std::unordered_map<std::string, std::shared_ptr<SensorReading>>& GetReadings() const {
        return readings_;
    }

    const std::unordered_map<std::string, float*>& GetReadingsValues() const {
        return readings_values_;
    }

    void ClearReadings() {
        readings_.clear();
        readings_values_.clear();
    }
};

} // eerie_leap::domain::sensor_domain::utilities
