#include <cstring>

#include "subsys/mdf/utilities/block_links_empty.h"

#include "block_base.h"

namespace eerie_leap::subsys::mdf::mdf4 {

using namespace eerie_leap::subsys::mdf::utilities;

BlockBase::BlockBase(const std::string& id): id_(id) { }

std::string BlockBase::GetId() const {
    return id_;
}

uint64_t BlockBase::GetBaseSize() const {
    return 4 + 4 + 8 + 8 + GetBlockLinks()->GetLinksSizeBytes();
}

std::unique_ptr<uint8_t[]> BlockBase::SerializeBase() const {
    const uint64_t size = GetBlockSize();
    auto buffer = std::make_unique<uint8_t[]>(size);
    std::memset(buffer.get(), 0, size);

    uint64_t offset = 0;

    auto id_char_array = "##" + id_;
    std::copy(id_char_array.begin(), id_char_array.end(), buffer.get() + offset);
    offset += 4;

    offset += 4; // reserved_0_

    uint64_t length = GetBlockSize();
    std::memcpy(buffer.get() + offset, &length, sizeof(length));
    offset += sizeof(length);

    const auto* block_links = GetBlockLinks();

    uint64_t link_count = block_links->Count();
    std::memcpy(buffer.get() + offset, &link_count, sizeof(link_count));
    offset += sizeof(link_count);

    if(link_count > 0) {
        auto links = block_links->Serialize();
        std::copy(links.get(), links.get() + block_links->GetLinksSizeBytes(), buffer.get() + offset);
        offset += block_links->GetLinksSizeBytes();
    }

    return buffer;
}

const IBlockLinks* BlockBase::GetBlockLinks() const {
    static const BlockLinksEmpty links_empty;
    return &links_empty;
}

} // namespace eerie_leap::subsys::mdf::mdf4
