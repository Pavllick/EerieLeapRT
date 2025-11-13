#pragma once

#include <cstdint>
#include <string>

#include "utilities/string/string_helpers.h"

namespace eerie_leap::domain::sensor_domain::models::sources {

using namespace eerie_leap::utilities::string;

// NOTE:: connection_string format:
// "bus_channel/frame_id</signal_name>"
struct CanbusSource {
    uint8_t bus_channel;
    uint32_t frame_id;
    std::string signal_name;
    size_t signal_name_hash = 0;

    CanbusSource(uint8_t bus_channel, uint32_t frame_id, const std::string& signal_name)
        : bus_channel(bus_channel), frame_id(frame_id), signal_name(signal_name) {

        if(!signal_name.empty())
            signal_name_hash = StringHelpers::GetHash(signal_name);
    }

    CanbusSource(uint8_t bus_channel, uint32_t frame_id)
        : CanbusSource(bus_channel, frame_id, "") {}

    std::string ToConnectionString() const {
        std::string connection_string = std::to_string(bus_channel);
        connection_string += "/" + std::to_string(frame_id);
        if(!signal_name.empty())
            connection_string += "/" + signal_name;

        return connection_string;
    }

    static CanbusSource FromConnectionString(const std::string& connection_string) {
        if(connection_string.empty())
            throw std::invalid_argument("Invalid format: empty connection string");

        size_t delimiter_pos = connection_string.find('/');

        if(delimiter_pos == std::string::npos)
            throw std::invalid_argument("Invalid CANBus connection string format");

        uint8_t bus_channel_value = 0;
        uint32_t frame_id_value = 0;
        std::string signal_name_value;

        bus_channel_value = std::stoul(connection_string.substr(0, delimiter_pos), nullptr, 0);
        std::string connection_str = connection_string.substr(delimiter_pos + 1);
        delimiter_pos = connection_str.find('/');

        if(delimiter_pos == std::string::npos) {
            frame_id_value = std::stoul(connection_str, nullptr, 0);
        } else {
            frame_id_value = std::stoul(connection_str.substr(0, delimiter_pos), nullptr, 0);
            signal_name_value = connection_str.substr(delimiter_pos + 1);
        }

        return {bus_channel_value, frame_id_value, signal_name_value};
    }
};

}

// namespace eerie_leap::domain::sensor_domain::models::sources
