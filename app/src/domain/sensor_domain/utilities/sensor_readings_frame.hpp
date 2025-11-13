#pragma once

#include <unordered_map>
#include <string>
#include <stdexcept>
#include <memory>

#include <zephyr/spinlock.h>

#include "domain/sensor_domain/models/sensor_reading.h"

namespace eerie_leap::domain::sensor_domain::utilities {

using namespace eerie_leap::domain::sensor_domain::models;

class SensorReadingsFrame {
private:
    std::unordered_map<std::string, std::shared_ptr<SensorReading>> readings_;
    std::unordered_map<std::string, float*> reading_values_;

    k_spinlock reading_lock_;

public:
    SensorReadingsFrame() = default;

    void AddOrUpdateReading(std::shared_ptr<SensorReading> reading) {
        std::string sensor_id = reading->sensor->id;

        auto lock_key = k_spin_lock(&reading_lock_);

        readings_[sensor_id] = reading;
        if(readings_[sensor_id]->status == ReadingStatus::PROCESSED && readings_[sensor_id]->value.has_value())
            reading_values_[sensor_id] = &readings_[sensor_id]->value.value();

        k_spin_unlock(&reading_lock_, lock_key);
    }

    bool HasReading(const std::string& sensor_id) const {
        return readings_.contains(sensor_id);
    }

    std::shared_ptr<SensorReading> GetReading(const std::string& sensor_id) const {
        if(!HasReading(sensor_id))
            throw std::runtime_error("Sensor ID not found");

        return readings_.at(sensor_id);
    }

    const std::unordered_map<std::string, std::shared_ptr<SensorReading>>& GetReadings() const {
        return readings_;
    }

    const std::unordered_map<std::string, float*>& GetReadingValues() const {
        return reading_values_;
    }

    void ClearReadings() {
        auto lock_key = k_spin_lock(&reading_lock_);
        readings_.clear();
        reading_values_.clear();
        k_spin_unlock(&reading_lock_, lock_key);
    }
};

} // eerie_leap::domain::sensor_domain::utilities
