#pragma once

#include "cbor_trait.h"
#include <configuration/cbor/cbor_sensors_config/cbor_sensors_config.h>
#include "configuration/cbor/cbor_sensors_config/cbor_sensors_config_cbor_encode.h"
#include "configuration/cbor/cbor_sensors_config/cbor_sensors_config_cbor_decode.h"
#include "configuration/cbor/cbor_sensors_config/cbor_sensors_config_size.h"

namespace eerie_leap::configuration::cbor::traits {

template <>
struct CborTrait<CborSensorsConfig> {
    static constexpr auto Encode = cbor_encode_CborSensorsConfig;
    static constexpr auto Decode = cbor_decode_CborSensorsConfig;
    static constexpr auto GetSerializingSize = cbor_get_size_CborSensorsConfig;
};

} // namespace eerie_leap::configuration::cbor::traits
