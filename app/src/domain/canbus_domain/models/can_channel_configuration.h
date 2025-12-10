#pragma once

#include <memory_resource>
#include <cstdint>
#include <string>
#include <vector>

#include "utilities/memory/memory_resource_manager.h"
#include "subsys/canbus/canbus_type.h"
#include "subsys/dbc/dbc.h"

#include "can_message_configuration.h"

namespace eerie_leap::domain::canbus_domain::models {

using namespace eerie_leap::utilities::memory;
using namespace eerie_leap::subsys::canbus;
using namespace eerie_leap::subsys::dbc;

struct CanChannelConfiguration {
    using allocator_type = std::pmr::polymorphic_allocator<std::byte>;

    CanbusType type;
    bool is_extended_id = false;
    uint8_t bus_channel;
    uint32_t bitrate;
    uint32_t data_bitrate;
    std::pmr::string dbc_file_path;
    std::pmr::vector<std::shared_ptr<CanMessageConfiguration>> message_configurations;

    std::shared_ptr<Dbc> dbc;

    CanChannelConfiguration(std::allocator_arg_t, const allocator_type& alloc)
        : dbc_file_path(alloc),
        message_configurations(alloc),
        dbc(make_shared_pmr<Dbc>(alloc)) {}

    CanChannelConfiguration(const CanChannelConfiguration&) = delete;
    CanChannelConfiguration& operator=(const CanChannelConfiguration&) = delete;

    CanChannelConfiguration(CanChannelConfiguration&& other) noexcept
        : type(other.type),
        is_extended_id(other.is_extended_id),
        bus_channel(other.bus_channel),
        bitrate(other.bitrate),
        data_bitrate(other.data_bitrate),
        dbc_file_path(std::move(other.dbc_file_path)),
        message_configurations(std::move(other.message_configurations)),
        dbc(std::move(other.dbc)) {}

    CanChannelConfiguration(CanChannelConfiguration&& other, const allocator_type& alloc)
        : type(other.type),
        is_extended_id(other.is_extended_id),
        bus_channel(other.bus_channel),
        bitrate(other.bitrate),
        data_bitrate(other.data_bitrate),
        dbc_file_path(std::move(other.dbc_file_path), alloc),
        message_configurations(std::move(other.message_configurations), alloc),
        dbc(std::move(other.dbc)) {}
};

} // namespace eerie_leap::domain::canbus_domain::models
