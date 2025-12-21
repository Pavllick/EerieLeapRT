#include "configuration/cbor/cbor_trait.h"
#include <configuration/cbor/cbor_sensors_config/cbor_sensors_config.h>
#include "configuration/cbor/cbor_sensors_config/cbor_sensors_config_cbor_encode.h"
#include "configuration/cbor/cbor_sensors_config/cbor_sensors_config_cbor_decode.h"
#include "configuration/cbor/cbor_sensors_config/cbor_sensors_config_size.h"

namespace eerie_leap::configuration::cbor::traits {

struct CborSensorsConfigTraitRegistrar {
    CborSensorsConfigTraitRegistrar() {
        CborTraitRegistry::Register<CborSensorsConfig>(
            cbor_encode_CborSensorsConfig,
            cbor_decode_CborSensorsConfig,
            cbor_get_size_CborSensorsConfig
        );
    }
} CborSensorsConfigTraitRegistrar;

} // namespace eerie_leap::configuration::cbor::traits
