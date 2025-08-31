#pragma once

#include <cstdint>

#include <zephyr/sys/byteorder.h>
#include <zephyr/sys/crc.h>

#include "utilities/constants.h"

namespace eerie_leap::domain::logging_domain::models {

using namespace eerie_leap::utilities::constants::logging;

// LogDataRecord<std::array<uint8_t, 9>> record;

template<typename T>
struct LogDataRecord {
    uint32_t record_start_mark;
    uint32_t timestamp_delta;
    uint32_t sensor_id;
    uint8_t data_length;
    T data;
    uint32_t crc;

    LogDataRecord() : data_length(sizeof(T)) {}

    static LogDataRecord Create(uint32_t timestamp_delta, uint32_t sensor_id, T data) {
        LogDataRecord record;
        record.record_start_mark = LOG_DATA_RECORD_START_MARK;
        record.timestamp_delta = timestamp_delta;
        record.sensor_id = sensor_id;
        record.data = data;
        record.crc = sys_cpu_to_le16(
            crc32_ieee((uint8_t*)(&record), sizeof(record) - sizeof(record.crc)));

        return record;
    }
} __attribute__((packed, aligned(1)));

} // namespace eerie_leap::domain::logging_domain::models
