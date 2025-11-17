#pragma once

#include <unordered_map>
#include <stdexcept>
#include <memory>

#include <zephyr/spinlock.h>

#include "domain/sensor_domain/models/sensor_reading.h"

namespace eerie_leap::domain::sensor_domain::utilities {

using namespace eerie_leap::domain::sensor_domain::models;

class SensorReadingsFrame {
private:
    std::unordered_map<size_t, std::shared_ptr<SensorReading>> readings_;
    std::unordered_map<size_t, float*> reading_values_;

    k_spinlock reading_lock_;

public:
    SensorReadingsFrame() = default;

    void AddOrUpdateReading(std::shared_ptr<SensorReading> reading) {
        auto lock_key = k_spin_lock(&reading_lock_);

        size_t sensor_id_hash = reading->sensor->id_hash;

        readings_[sensor_id_hash] = reading;
        if(readings_[sensor_id_hash]->status == ReadingStatus::PROCESSED && readings_[sensor_id_hash]->value.has_value())
            reading_values_[sensor_id_hash] = &readings_[sensor_id_hash]->value.value();

        k_spin_unlock(&reading_lock_, lock_key);
    }

    bool HasReading(const size_t sensor_id_hash) const {
        return readings_.contains(sensor_id_hash);
    }

    std::shared_ptr<SensorReading> GetReading(const size_t sensor_id_hash) const {
        if(!HasReading(sensor_id_hash))
            throw std::runtime_error("Sensor ID not found");

        return readings_.at(sensor_id_hash);
    }

    float GetReadingValue(const size_t sensor_id_hash) const {
        if(!HasReading(sensor_id_hash))
            throw std::runtime_error("Sensor ID not found");

        return *reading_values_.at(sensor_id_hash);
    }

    const std::unordered_map<size_t, std::shared_ptr<SensorReading>>& GetReadings() const {
        return readings_;
    }

    const std::unordered_map<size_t, float*>& GetReadingValues() const {
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
