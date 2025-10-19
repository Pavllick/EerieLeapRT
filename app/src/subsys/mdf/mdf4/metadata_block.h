#pragma once

#include "subsys/mdf/i_block.h"
#include "text_block_base.h"

namespace eerie_leap::subsys::mdf::mdf4 {

class MetadataBlock : public TextBlockBase {
public:
    MetadataBlock(): TextBlockBase("MD") {}
    virtual ~MetadataBlock() = default;

    void SetText(const std::string& block_id, const std::string& text);
};

} // namespace eerie_leap::subsys::mdf::mdf4
