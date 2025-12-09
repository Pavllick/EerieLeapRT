#pragma once

#include <unordered_map>
#include <stdexcept>
#include <memory>
#include <string>

#include <zephyr/spinlock.h>

#include "utilities/string/string_helpers.h"
#include "domain/sensor_domain/models/sensor_reading.h"

namespace eerie_leap::domain::sensor_domain::utilities {

using namespace eerie_leap::utilities::string;
using namespace eerie_leap::domain::sensor_domain::models;

class SensorReadingsFrame {
private:
    std::unordered_map<size_t, std::shared_ptr<SensorReading>> readings_;
    std::unordered_map<size_t, float> reading_values_;
    mutable std::unordered_map<std::string, size_t> sensor_id_hash_map_;

    k_spinlock reading_lock_;

    size_t GetSensorIdHash(const std::string& sensor_id) const {
        if(!sensor_id_hash_map_.contains(sensor_id))
            sensor_id_hash_map_.emplace(sensor_id, StringHelpers::GetHash(sensor_id));

        return sensor_id_hash_map_.at(sensor_id);
    }

    static inline void ErrorSensorIdNotFound() {
        throw std::runtime_error("Sensor ID not found");
    }

public:
    SensorReadingsFrame() = default;

    void AddOrUpdateReading(std::shared_ptr<SensorReading> reading) {
        auto lock_key = k_spin_lock(&reading_lock_);

        size_t sensor_id_hash = reading->sensor->id_hash;

        readings_[sensor_id_hash] = std::move(reading);
        if(readings_[sensor_id_hash]->status == ReadingStatus::PROCESSED && readings_[sensor_id_hash]->value.has_value())
            reading_values_[sensor_id_hash] = readings_[sensor_id_hash]->value.value();

        k_spin_unlock(&reading_lock_, lock_key);
    }

    bool HasReading(const size_t sensor_id_hash) const {
        return readings_.contains(sensor_id_hash);
    }

    bool HasReading(const std::string& sensor_id) const {
        return readings_.contains(GetSensorIdHash(sensor_id));
    }

    bool HasReadingValue(const size_t sensor_id_hash) const {
        return reading_values_.contains(sensor_id_hash);
    }

    bool HasReadingValue(const std::string& sensor_id) const {
        return reading_values_.contains(GetSensorIdHash(sensor_id));
    }

    std::shared_ptr<SensorReading> GetReading(const size_t sensor_id_hash) const {
        if(!HasReading(sensor_id_hash))
            ErrorSensorIdNotFound();

        return readings_.at(sensor_id_hash);
    }

    std::shared_ptr<SensorReading> GetReading(const std::string& sensor_id) const {
        const size_t sensor_id_hash = GetSensorIdHash(sensor_id);

        if(!HasReading(sensor_id_hash))
            ErrorSensorIdNotFound();

        return readings_.at(sensor_id_hash);
    }

    float GetReadingValue(const size_t sensor_id_hash) const {
        if(!HasReadingValue(sensor_id_hash))
            ErrorSensorIdNotFound();

        return reading_values_.at(sensor_id_hash);
    }

    float GetReadingValue(const std::string& sensor_id) const {
        const size_t sensor_id_hash = GetSensorIdHash(sensor_id);

        if(!HasReadingValue(sensor_id_hash))
            ErrorSensorIdNotFound();

        return reading_values_.at(sensor_id_hash);
    }

    float* GetReadingValuePtr(const std::string& sensor_id) {
        const size_t sensor_id_hash = GetSensorIdHash(sensor_id);

        if(!HasReadingValue(sensor_id_hash))
            ErrorSensorIdNotFound();

        return &reading_values_.at(sensor_id_hash);
    }

    const std::unordered_map<size_t, std::shared_ptr<SensorReading>>& GetReadings() const {
        return readings_;
    }

    void ClearReadings() {
        auto lock_key = k_spin_lock(&reading_lock_);
        readings_.clear();
        reading_values_.clear();
        k_spin_unlock(&reading_lock_, lock_key);
    }
};

} // eerie_leap::domain::sensor_domain::utilities
