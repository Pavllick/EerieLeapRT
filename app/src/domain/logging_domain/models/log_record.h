#pragma once

#include <cstdint>

#include <zephyr/sys/byteorder.h>
#include <zephyr/sys/crc.h>

namespace eerie_leap::domain::logging_domain::models {

// LogRecord<std::array<uint8_t, 9>> record;

template<typename T>
struct LogRecord {
    uint32_t timestamp_delta;
    uint32_t sensor_id;
    uint8_t data_length;
    T data;
    uint32_t crc;

    LogRecord() : data_length(sizeof(T)) {}

    static LogRecord Create(uint32_t timestamp_delta, uint32_t sensor_id, T data) {
        LogRecord record;
        record.timestamp_delta = timestamp_delta;
        record.sensor_id = sensor_id;
        record.data = data;
        record.crc = sys_cpu_to_le16(
            crc32_ieee((uint8_t*)(&record), sizeof(record) - sizeof(record.crc)));

        return record;
    }
} __attribute__((packed, aligned(1)));

}
