#include "configuration/cbor/cbor_trait.h"
#include <configuration/cbor/cbor_adc_config/cbor_adc_config.h>
#include "configuration/cbor/cbor_adc_config/cbor_adc_config_cbor_encode.h"
#include "configuration/cbor/cbor_adc_config/cbor_adc_config_cbor_decode.h"
#include "configuration/cbor/cbor_adc_config/cbor_adc_config_size.h"

namespace eerie_leap::configuration::cbor::traits {

struct CborAdcConfigTraitRegistrar {
    CborAdcConfigTraitRegistrar() {
        CborTraitRegistry::Register<CborAdcConfig>(
            cbor_encode_CborAdcConfig,
            cbor_decode_CborAdcConfig,
            cbor_get_size_CborAdcConfig
        );
    }
} CborAdcConfigTraitRegistrar;

} // namespace eerie_leap::configuration::cbor::traits
