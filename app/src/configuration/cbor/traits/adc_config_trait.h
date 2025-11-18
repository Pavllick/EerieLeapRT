#pragma once

#include "cbor_trait.h"
#include <configuration/cbor/cbor_adc_config/cbor_adc_config.h>
#include "configuration/cbor/cbor_adc_config/cbor_adc_config_cbor_encode.h"
#include "configuration/cbor/cbor_adc_config/cbor_adc_config_cbor_decode.h"
#include "configuration/cbor/cbor_adc_config/cbor_adc_config_size.h"

namespace eerie_leap::configuration::cbor::traits {

template <>
struct CborTrait<CborAdcConfig> {
    static constexpr auto Encode = cbor_encode_CborAdcConfig;
    static constexpr auto Decode = cbor_decode_CborAdcConfig;
    static constexpr auto GetSerializingSize = cbor_get_size_CborAdcConfig;
};

} // namespace eerie_leap::configuration::cbor::traits
