#pragma once

#include "cbor_trait.h"
#include <configuration/sensor_config/sensor_config.h>
#include "configuration/sensor_config/sensor_config_cbor_encode.h"
#include "configuration/sensor_config/sensor_config_cbor_decode.h"

namespace eerie_leap::configuration::traits {

template <>
struct CborTrait<SensorsConfig> {
    static constexpr auto Encode = cbor_encode_SensorsConfig;
    static constexpr auto Decode = cbor_decode_SensorsConfig;
};

} // namespace eerie_leap::configuration::traits