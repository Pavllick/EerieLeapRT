#include "configuration/cbor/cbor_trait.h"
#include <configuration/cbor/cbor_logging_config/cbor_logging_config.h>
#include "configuration/cbor/cbor_logging_config/cbor_logging_config_cbor_encode.h"
#include "configuration/cbor/cbor_logging_config/cbor_logging_config_cbor_decode.h"
#include "configuration/cbor/cbor_logging_config/cbor_logging_config_size.h"

namespace eerie_leap::configuration::cbor::traits {

struct CborLoggingConfigTraitRegistrar {
    CborLoggingConfigTraitRegistrar() {
        CborTraitRegistry::Register<CborLoggingConfig>(
            cbor_encode_CborLoggingConfig,
            cbor_decode_CborLoggingConfig,
            cbor_get_size_CborLoggingConfig
        );
    }
} CborLoggingConfigTraitRegistrar;

} // namespace eerie_leap::configuration::cbor::traits
