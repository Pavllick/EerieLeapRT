#pragma once

#include "cbor_trait.h"
#include <configuration/adc_config/adc_config.h>
#include "configuration/adc_config/adc_config_cbor_encode.h"
#include "configuration/adc_config/adc_config_cbor_decode.h"

namespace eerie_leap::configuration::traits {

template <>
struct CborTrait<AdcConfig> {
    static constexpr auto Encode = cbor_encode_AdcConfig;
    static constexpr auto Decode = cbor_decode_AdcConfig;
};

} // namespace eerie_leap::configuration::traits
