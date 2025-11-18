#pragma once

#include <zephyr/data/json.h>

#include "configuration/json/configs/json_system_config.h"
#include "json_trait.h"

namespace eerie_leap::configuration::json::traits {

using namespace eerie_leap::configuration::json::configs;

template <>
struct JsonTrait<JsonSystemConfig> {
    static constexpr auto Encode = json_encode_JsonSystemConfig;
    static constexpr auto Decode = json_decode_JsonSystemConfig;
};

} // namespace eerie_leap::configuration::json::traits
