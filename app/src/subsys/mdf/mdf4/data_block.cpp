#include <cstdint>
#include <memory>

#include "data_block.h"

namespace eerie_leap::subsys::mdf::mdf4 {

DataBlock::DataBlock(size_t size_bytes): BlockBase("DT"), size_bytes_(size_bytes) {}

uint64_t DataBlock::GetBlockSize() const {
    return GetBaseSize() + size_bytes_;
}

std::unique_ptr<uint8_t[]> DataBlock::Serialize() const {
    return SerializeBase();
}

} // namespace eerie_leap::subsys::mdf::mdf4
