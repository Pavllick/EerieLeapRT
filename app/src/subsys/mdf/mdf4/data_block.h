#pragma once

#include <cstdint>
#include <memory>

#include "block_base.h"

namespace eerie_leap::subsys::mdf::mdf4 {

class DataBlock : public BlockBase {
private:
    size_t size_;

public:
    DataBlock(size_t size = 0): BlockBase("DT"), size_(size) {}
    virtual ~DataBlock() = default;

    uint64_t GetSize() const override { return GetBaseSize() + size_; }
    std::unique_ptr<uint8_t[]> Serialize() const override { return SerializeBase(); }
    std::vector<std::shared_ptr<ISerializableBlock>> GetChildren() const override { return {}; }
};

} // namespace eerie_leap::subsys::mdf::mdf4
