#pragma once

#include <cstdint>
#include <memory>

namespace eerie_leap::subsys::mdf::utilities {

class IBlockLinks {
public:
    IBlockLinks() = default;

    virtual int Count() const = 0;
    virtual uint64_t GetLinksSizeBytes() const = 0;
    virtual std::unique_ptr<uint8_t[]> Serialize() const = 0;
};

} // namespace eerie_leap::subsys::mdf::utilities
