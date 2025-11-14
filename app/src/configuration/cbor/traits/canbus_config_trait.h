#pragma once

#include "cbor_trait.h"
#include <configuration/cbor/cbor_canbus_config/cbor_canbus_config.h>
#include "configuration/cbor/cbor_canbus_config/canbus_config_cbor_encode.h"
#include "configuration/cbor/cbor_canbus_config/canbus_config_cbor_decode.h"
#include "configuration/cbor/cbor_canbus_config/canbus_config_cbor_size.h"

namespace eerie_leap::configuration::cbor::traits {

template <>
struct CborTrait<CborCanbusConfig> {
    static constexpr auto Encode = cbor_encode_CborCanbusConfig;
    static constexpr auto Decode = cbor_decode_CborCanbusConfig;
    static constexpr auto GetSerializingSize = cbor_get_size_CborCanbusConfig;
};

} // namespace eerie_leap::configuration::cbor::traits
