#pragma once

#include <cstdint>
#include <sstream>
#include <fstream>

#include "i_serializable_block.h"

namespace eerie_leap::subsys::mdf {

class SerializableBlockBase : public virtual ISerializableBlock {
protected:
    uint64_t address_;

public:
    SerializableBlockBase();

    uint64_t WriteToStream(std::streambuf& stream) const override;
    uint64_t GetAddress() const override;
    uint64_t ResolveAddress(uint64_t parent_address) override;
};

} // namespace eerie_leap::subsys::mdf
