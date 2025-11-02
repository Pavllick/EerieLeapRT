#pragma once

#include <cstdint>

namespace eerie_leap::subsys::mdf {

enum class MdfDataType : uint8_t {
    None,
    Int32,
    Int64,
    Uint32,
    Uint64,
    Float32,
    Float64,
    ByteArray
};

} // namespace eerie_leap::subsys::mdf
