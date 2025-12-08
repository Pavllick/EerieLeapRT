#pragma once

#include <cstdint>
#include <memory>
#include <vector>

#include "subsys/mdf/i_serializable_block.h"

namespace eerie_leap::subsys::mdf::utilities {

class IBlockLinks {
public:
    IBlockLinks() = default;
    virtual ~IBlockLinks() = default;

    virtual int Count() const = 0;
    virtual uint64_t GetLinksSizeBytes() const = 0;
    virtual const std::vector<std::shared_ptr<ISerializableBlock>> GetLinks() const = 0;
    virtual std::unique_ptr<uint8_t[]> Serialize() const = 0;
};

} // namespace eerie_leap::subsys::mdf::utilities
