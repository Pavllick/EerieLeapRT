#pragma once

#include <zephyr/data/json.h>

#include "configuration/json/configs/json_sensors_config.h"
#include "json_trait.h"

namespace eerie_leap::configuration::json::traits {

using namespace eerie_leap::configuration::json::configs;

template <>
struct JsonTrait<JsonSensorsConfig> {
    static constexpr json_obj_descr* object_descriptor = json_sensors_descr;
    static constexpr size_t object_descriptor_size = ARRAY_SIZE(json_sensors_descr);
};

} // namespace eerie_leap::configuration::json::traits
