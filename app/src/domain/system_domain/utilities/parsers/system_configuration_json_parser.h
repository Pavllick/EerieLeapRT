#pragma once

#include <span>

#include <zephyr/data/json.h>

#include "utilities/memory/heap_allocator.h"
#include "domain/system_domain/models/system_configuration.h"

namespace eerie_leap::domain::system_domain::utilities::parsers {

using namespace eerie_leap::utilities::memory;
using namespace eerie_leap::domain::system_domain::models;

struct ComUserConfigurationJsonDto {
    uint64_t device_id;
    uint32_t server_id;
};

struct CanbusConfigurationJsonDto {
    uint32_t bus_channel;
    uint32_t bitrate;
};

struct SystemConfigurationJsonDto {
    uint32_t com_user_refresh_rate_ms;
    ComUserConfigurationJsonDto com_user_configurations[8];
    size_t com_user_configurations_len;
    CanbusConfigurationJsonDto canbus_configurations[8];
    size_t canbus_configurations_len;
};

static json_obj_descr com_user_configuration_descr[] = {
    JSON_OBJ_DESCR_PRIM(ComUserConfigurationJsonDto, device_id, JSON_TOK_UINT64),
    JSON_OBJ_DESCR_PRIM(ComUserConfigurationJsonDto, server_id, JSON_TOK_UINT),
};

static json_obj_descr canbus_configuration_descr[] = {
    JSON_OBJ_DESCR_PRIM(CanbusConfigurationJsonDto, bus_channel, JSON_TOK_UINT),
    JSON_OBJ_DESCR_PRIM(CanbusConfigurationJsonDto, bitrate, JSON_TOK_UINT),
};

static json_obj_descr system_configuration_descr[] = {
    JSON_OBJ_DESCR_PRIM(SystemConfigurationJsonDto, com_user_refresh_rate_ms, JSON_TOK_UINT),
    JSON_OBJ_DESCR_OBJ_ARRAY(SystemConfigurationJsonDto, com_user_configurations, 8, com_user_configurations_len, com_user_configuration_descr, ARRAY_SIZE(com_user_configuration_descr)),
    JSON_OBJ_DESCR_OBJ_ARRAY(SystemConfigurationJsonDto, canbus_configurations, 8, canbus_configurations_len, canbus_configuration_descr, ARRAY_SIZE(canbus_configuration_descr)),
};

class SystemConfigurationJsonParser {
public:
    SystemConfigurationJsonParser() = default;

    ext_unique_ptr<ExtVector> Serialize(const SystemConfiguration& system_configuration);
    SystemConfiguration Deserialize(
        const std::span<const uint8_t>& json,
        uint64_t device_id,
        uint32_t hw_version,
        uint32_t sw_version,
        uint32_t build_number);
};

} // namespace eerie_leap::domain::system_domain::utilities::parsers
