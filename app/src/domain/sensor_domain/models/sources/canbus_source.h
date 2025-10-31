#pragma once

#include <cstdint>

namespace eerie_leap::domain::sensor_domain::models::sources {

// NOTE:: connection_string format: "frame_id/<signal_name>"
struct CanbusSource {
    uint32_t frame_id;
    std::string signal_name;

    std::string ToConnectionString() const {
        std::string connection_string = std::to_string(frame_id);
        if(!signal_name.empty())
            connection_string += "/" + signal_name;

        return connection_string;
    }

    static CanbusSource FromConnectionString(const std::string& connection_string) {
        if(connection_string.empty())
            throw std::invalid_argument("Invalid format: empty connection string");

        size_t delimiter_pos = connection_string.find('/');

        CanbusSource source;

        if(delimiter_pos == std::string::npos) {
            source.frame_id = std::stoul(connection_string, nullptr, 0);
            source.signal_name = "";
        } else {
            std::string frame_id_str = connection_string.substr(0, delimiter_pos);
            source.frame_id = std::stoul(frame_id_str, nullptr, 0);
            source.signal_name = connection_string.substr(delimiter_pos + 1);
        }

        return source;
    }
};

}

// namespace eerie_leap::domain::sensor_domain::models::sources
