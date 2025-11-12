#pragma once

#include "cbor_trait.h"
#include <configuration/cbor_sensor_config/cbor_sensor_config.h>
#include "configuration/cbor_sensor_config/sensor_config_cbor_encode.h"
#include "configuration/cbor_sensor_config/sensor_config_cbor_decode.h"
#include "configuration/cbor_sensor_config/sensor_config_cbor_size.h"

namespace eerie_leap::configuration::traits {

template <>
struct CborTrait<CborSensorsConfig> {
    static constexpr auto Encode = cbor_encode_CborSensorsConfig;
    static constexpr auto Decode = cbor_decode_CborSensorsConfig;
    static constexpr auto GetSerializingSize = cbor_get_size_CborSensorsConfig;
};

} // namespace eerie_leap::configuration::traits
