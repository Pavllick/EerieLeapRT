#pragma once

#include "cbor_trait.h"
#include <configuration/logging_config/logging_config.h>
#include "configuration/logging_config/logging_config_cbor_encode.h"
#include "configuration/logging_config/logging_config_cbor_decode.h"
#include "configuration/logging_config/logging_config_cbor_size.h"

namespace eerie_leap::configuration::traits {

template <>
struct CborTrait<LoggingConfig> {
    static constexpr auto Encode = cbor_encode_LoggingConfig;
    static constexpr auto Decode = cbor_decode_LoggingConfig;
    static constexpr auto GetSerializingSize = cbor_get_size_LoggingConfig;
};

} // namespace eerie_leap::configuration::traits
