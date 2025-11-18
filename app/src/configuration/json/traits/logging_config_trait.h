#pragma once

#include <zephyr/data/json.h>

#include "configuration/json/configs/json_logging_config.h"
#include "json_trait.h"

namespace eerie_leap::configuration::json::traits {

using namespace eerie_leap::configuration::json::configs;

template <>
struct JsonTrait<JsonLoggingConfig> {
    static constexpr auto Encode = json_encode_JsonLoggingConfig;
    static constexpr auto Decode = json_decode_JsonLoggingConfig;
};

} // namespace eerie_leap::configuration::json::traits
