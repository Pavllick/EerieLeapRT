#pragma once

#include <string>

#include "subsys/mdf/utilities/i_block_links.h"
#include "i_serializable_block.h"

namespace eerie_leap::subsys::mdf {

using namespace eerie_leap::subsys::mdf::utilities;

class IBlock : public virtual ISerializableBlock {
public:
    virtual ~IBlock() = default;
    virtual std::string GetId() const = 0;
    virtual const IBlockLinks* GetBlockLinks() const = 0;
};

} // namespace eerie_leap::subsys::mdf
