#pragma once

#include <zephyr/data/json.h>

#include "configuration/json/configs/json_logging_config.h"
#include "json_trait.h"

namespace eerie_leap::configuration::json::traits {

using namespace eerie_leap::configuration::json::configs;

template <>
struct JsonTrait<JsonLoggingConfig> {
    static constexpr json_obj_descr* object_descriptor = json_logging_config_descr;
    static constexpr size_t object_descriptor_size = ARRAY_SIZE(json_logging_config_descr);
};

} // namespace eerie_leap::configuration::json::traits
