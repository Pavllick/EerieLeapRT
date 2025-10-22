#pragma once

#include <cstdint>
#include <memory>

#include "block_base.h"

namespace eerie_leap::subsys::mdf::mdf4 {

class DataBlock : public BlockBase {
private:
    size_t size_bytes_;

public:
    DataBlock(size_t size_bytes = 0);
    virtual ~DataBlock() = default;

    uint64_t GetBlockSize() const override;
    std::unique_ptr<uint8_t[]> Serialize() const override;
    std::vector<std::shared_ptr<ISerializableBlock>> GetChildren() const override {
        return {};
    }
};

} // namespace eerie_leap::subsys::mdf::mdf4
