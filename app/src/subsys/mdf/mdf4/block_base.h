#pragma once

#include <cstdint>
#include <string>

#include "subsys/mdf/i_block.h"
#include "subsys/mdf/serializable_block_base.h"

namespace eerie_leap::subsys::mdf::mdf4 {

class BlockBase : public SerializableBlockBase, public IBlock {
protected:
    std::string id_;                        // 4 bytes, File identifier
    // uint8_t reserved_0_[4];              // 4 bytes, Reserved
    // uint64_t length_;                    // 8 bytes, Length of the block
    // uint64_t link_count_;                // 8 bytes, Number of links

public:
    BlockBase(const std::string& id);
    virtual ~BlockBase() = default;

    std::string GetId() const override;
    uint64_t GetBaseSize() const;
    std::unique_ptr<uint8_t[]> SerializeBase() const;
    const IBlockLinks* GetBlockLinks() const override;
};

} // namespace eerie_leap::subsys::mdf::mdf4
