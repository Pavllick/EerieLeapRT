#include "configuration/cbor/cbor_trait.h"
#include <configuration/cbor/cbor_canbus_config/cbor_canbus_config.h>
#include "configuration/cbor/cbor_canbus_config/cbor_canbus_config_cbor_encode.h"
#include "configuration/cbor/cbor_canbus_config/cbor_canbus_config_cbor_decode.h"
#include "configuration/cbor/cbor_canbus_config/cbor_canbus_config_size.h"

namespace eerie_leap::configuration::cbor::traits {

struct CborCanbusConfigTraitRegistrar {
    CborCanbusConfigTraitRegistrar() {
        CborTraitRegistry::Register<CborCanbusConfig>(
            cbor_encode_CborCanbusConfig,
            cbor_decode_CborCanbusConfig,
            cbor_get_size_CborCanbusConfig
        );
    }
} CborCanbusConfigTraitRegistrar;

} // namespace eerie_leap::configuration::cbor::traits
