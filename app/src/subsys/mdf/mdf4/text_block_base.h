#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include "block_base.h"

namespace eerie_leap::subsys::mdf::mdf4 {

class TextBlockBase : public BlockBase {
protected:
    std::string text_;

    size_t GetAllignedTextSize() const;

public:
    TextBlockBase(const std::string& id);
    virtual ~TextBlockBase() = default;

    uint64_t GetBlockSize() const override;
    std::unique_ptr<uint8_t[]> Serialize() const override;
    std::vector<std::shared_ptr<ISerializableBlock>> GetChildren() const override {
        return {};
    }
};

} // namespace eerie_leap::subsys::mdf::mdf4
