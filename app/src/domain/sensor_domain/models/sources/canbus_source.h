#pragma once

#include <cstdint>

namespace eerie_leap::domain::sensor_domain::models::sources {

// NOTE:: connection_string format:
// "bus_channel/frame_id</signal_name>"
struct CanbusSource {
    uint8_t bus_channel;
    uint32_t frame_id;
    std::string signal_name;

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

        CanbusSource source;

        source.bus_channel = std::stoul(connection_string.substr(0, delimiter_pos), nullptr, 0);
        std::string connection_str = connection_string.substr(delimiter_pos + 1);
        delimiter_pos = connection_str.find('/');

        if(delimiter_pos == std::string::npos) {
            source.frame_id = std::stoul(connection_str, nullptr, 0);
            source.signal_name = "";
        } else {
            source.frame_id = std::stoul(connection_str.substr(0, delimiter_pos), nullptr, 0);
            source.signal_name = connection_str.substr(delimiter_pos + 1);
        }

        return source;
    }
};

}

// namespace eerie_leap::domain::sensor_domain::models::sources
