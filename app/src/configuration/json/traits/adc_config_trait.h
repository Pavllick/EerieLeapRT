#pragma once

#include <zephyr/data/json.h>

#include "configuration/json/configs/json_adc_config.h"
#include "json_trait.h"

namespace eerie_leap::configuration::json::traits {

using namespace eerie_leap::configuration::json::configs;

template <>
struct JsonTrait<JsonAdcConfig> {
    static constexpr auto Encode = json_encode_JsonAdcConfig;
    static constexpr auto Decode = json_decode_JsonAdcConfig;
};

} // namespace eerie_leap::configuration::json::traits
