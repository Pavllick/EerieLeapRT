#pragma once

#include <memory_resource>
#include <cstdint>
#include <string>

#include "utilities/string/string_helpers.h"

namespace eerie_leap::domain::sensor_domain::models::sources {

using namespace eerie_leap::utilities::string;

// NOTE:: connection_string format:
// "bus_channel/frame_id</signal_name>"
struct CanbusSource {
    using allocator_type = std::pmr::polymorphic_allocator<>;

    uint8_t bus_channel;
    uint32_t frame_id;
    std::pmr::string signal_name;
    size_t signal_name_hash = 0;

    CanbusSource(std::allocator_arg_t, allocator_type alloc, uint8_t bus_channel, uint32_t frame_id, std::string_view signal_name)
        : bus_channel(bus_channel), frame_id(frame_id), signal_name(signal_name, alloc) {

        if(!signal_name.empty())
            signal_name_hash = StringHelpers::GetHash(signal_name);
    }

    CanbusSource(std::allocator_arg_t alloc_arg, allocator_type alloc, uint8_t bus_channel, uint32_t frame_id)
        : CanbusSource(alloc_arg, alloc, bus_channel, frame_id, "") {}


    CanbusSource(const CanbusSource&) = delete;
    CanbusSource& operator=(const CanbusSource&) = delete;

    CanbusSource(CanbusSource&&) noexcept = default;

    CanbusSource(CanbusSource&& other, allocator_type alloc) noexcept
        : bus_channel(other.bus_channel),
        frame_id(other.frame_id),
        signal_name(other.signal_name, alloc),
        signal_name_hash(other.signal_name_hash) {}

    std::string ToConnectionString() const {
        std::string connection_string = std::to_string(bus_channel);
        connection_string += "/" + std::to_string(frame_id);
        if(!signal_name.empty())
            connection_string += "/" + signal_name;

        return connection_string;
    }

    static CanbusSource FromConnectionString(allocator_type alloc, std::string_view connection_string) {
        if(connection_string.empty())
            throw std::invalid_argument("Invalid format: empty connection string");

        size_t delimiter_pos = connection_string.find('/');

        if(delimiter_pos == std::string::npos)
            throw std::invalid_argument("Invalid CANBus connection string format");

        uint8_t bus_channel_value = 0;
        uint32_t frame_id_value = 0;
        std::string signal_name_value;

        std::string connection_str = std::string(connection_string);
        bus_channel_value = std::stoul(connection_str.substr(0, delimiter_pos), nullptr, 0);
        connection_str = connection_str.substr(delimiter_pos + 1);
        delimiter_pos = connection_str.find('/');

        if(delimiter_pos == std::string::npos) {
            frame_id_value = std::stoul(connection_str, nullptr, 0);
        } else {
            frame_id_value = std::stoul(connection_str.substr(0, delimiter_pos), nullptr, 0);
            signal_name_value = connection_str.substr(delimiter_pos + 1);
        }

        return {std::allocator_arg, alloc, bus_channel_value, frame_id_value, signal_name_value};
    }
};

}

// namespace eerie_leap::domain::sensor_domain::models::sources
