#pragma once

#include <cstdint>
#include <memory>
#include <vector>

namespace eerie_leap::subsys::mdf {

class ISerializableBlock {
public:
    virtual ~ISerializableBlock() = default;

    virtual uint64_t GetBlockSize() const = 0;
    virtual std::unique_ptr<uint8_t[]> Serialize() const = 0;
    virtual uint64_t WriteToStream(std::streambuf& stream) = 0;
    virtual uint64_t GetAddress() const = 0;
    virtual bool IsSerialized() const = 0;
    virtual void Reset() = 0;
    virtual uint64_t ResolveAddress(uint64_t parent_address) = 0;

    virtual std::vector<std::shared_ptr<ISerializableBlock>> GetChildren() const = 0;
};

} // namespace eerie_leap::subsys::mdf
