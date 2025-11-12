#pragma once

#include <zephyr/data/json.h>

#include "configuration/json/configs/json_adc_config.h"
#include "json_trait.h"

namespace eerie_leap::configuration::json::traits {

using namespace eerie_leap::configuration::json::configs;

template <>
struct JsonTrait<JsonAdcConfig> {
    static constexpr json_obj_descr* object_descriptor = json_adc_config_descr;
    static constexpr size_t object_descriptor_size = ARRAY_SIZE(json_adc_config_descr);
};

} // namespace eerie_leap::configuration::json::traits
