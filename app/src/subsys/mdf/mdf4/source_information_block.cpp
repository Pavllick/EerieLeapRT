#include "source_information_block.h"

namespace eerie_leap::subsys::mdf::mdf4 {

SourceInformationBlock::SourceInformationBlock(SourceType source_type, BusType bus_type)
    : BlockBase("SI") {

    source_type_ = source_type;
    bus_type_ = bus_type;
    flags_ = 0;
}

void SourceInformationBlock::SetName(std::shared_ptr<TextBlock> name) {
    links_.SetLink(LinkType::TextName, name);
}

void SourceInformationBlock::SetPath(std::shared_ptr<TextBlock> path) {
    links_.SetLink(LinkType::TextPath, path);
}

uint64_t SourceInformationBlock::GetBlockSize() const {
    return GetBaseSize() + 1 + 1 + 1 + 5;
}

std::unique_ptr<uint8_t[]> SourceInformationBlock::Serialize() const {
    auto buffer = SerializeBase();
    uint64_t offset = GetBaseSize();

    std::memcpy(buffer.get() + offset, &source_type_, sizeof(uint8_t));
    offset += sizeof(uint8_t);

    std::memcpy(buffer.get() + offset, &bus_type_, sizeof(uint8_t));
    offset += sizeof(uint8_t);

    std::memcpy(buffer.get() + offset, &flags_, sizeof(flags_));
    offset += sizeof(flags_);

    offset += 5; // reserved_1_

    return buffer;
}

} // namespace eerie_leap::subsys::mdf::mdf4
