#pragma once

#include <zephyr/data/json.h>

#include "configuration/json/configs/json_canbus_config.h"
#include "json_trait.h"

namespace eerie_leap::configuration::json::traits {

using namespace eerie_leap::configuration::json::configs;

template <>
struct JsonTrait<JsonCanbusConfig> {
    static constexpr auto Encode = json_encode_JsonCanbusConfig;
    static constexpr auto Decode = json_decode_JsonCanbusConfig;
};

} // namespace eerie_leap::configuration::json::traits
