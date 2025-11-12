#pragma once

#include "cbor_trait.h"
#include <configuration/cbor_logging_config/cbor_logging_config.h>
#include "configuration/cbor_logging_config/logging_config_cbor_encode.h"
#include "configuration/cbor_logging_config/logging_config_cbor_decode.h"
#include "configuration/cbor_logging_config/logging_config_cbor_size.h"

namespace eerie_leap::configuration::traits {

template <>
struct CborTrait<CborLoggingConfig> {
    static constexpr auto Encode = cbor_encode_CborLoggingConfig;
    static constexpr auto Decode = cbor_decode_CborLoggingConfig;
    static constexpr auto GetSerializingSize = cbor_get_size_CborLoggingConfig;
};

} // namespace eerie_leap::configuration::traits
