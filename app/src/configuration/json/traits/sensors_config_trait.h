#pragma once

#include <zephyr/data/json.h>

#include "configuration/json/configs/json_sensors_config.h"
#include "json_trait.h"

namespace eerie_leap::configuration::json::traits {

using namespace eerie_leap::configuration::json::configs;

template <>
struct JsonTrait<JsonSensorsConfig> {
    static constexpr auto Encode = json_encode_JsonSensorsConfig;
    static constexpr auto Decode = json_decode_JsonSensorsConfig;
};

} // namespace eerie_leap::configuration::json::traits
