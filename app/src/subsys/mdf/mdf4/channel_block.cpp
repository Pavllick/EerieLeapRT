#include "channel_block.h"

namespace eerie_leap::subsys::mdf::mdf4 {

ChannelBlock::ChannelBlock(Type type, SyncType sync_type, DataType data_type, std::string name, std::string unit, size_t bit_count): BlockBase("CN") {
    type_ = type;
    sync_type_ = sync_type;
    data_type_ = data_type;
    bit_offset_ = 0;
    byte_offset_ = 0;
    bit_count_ = bit_count > 0 ? bit_count : GetBitCountForType(data_type_);
    flags_ = 0;
    invalidation_bit_pos_ = 0;
    precision_ = 0;
    attachment_count_ = 0;
    val_range_min_ = 0;
    val_range_max_ = 0;
    limit_min_ = 0;
    limit_max_ = 0;
    limit_ext_min_ = 0;
    limit_ext_max_ = 0;

    name_ = std::make_shared<TextBlock>();
    name_->SetText(name);
    links_.SetLink(LinkType::TextName, name_);

    unit_ = std::make_shared<TextBlock>();
    unit_->SetText(unit);
    links_.SetLink(LinkType::TextUnit, unit_);
}

uint32_t ChannelBlock::GetDataBytes() const {
    return bit_count_ / 8;
}

void ChannelBlock::SetOffsetBytes(uint32_t offset_bytes) {
    byte_offset_ = offset_bytes;
}

void ChannelBlock::LinkBlock(std::shared_ptr<ChannelBlock> next_block) {
    if(channel_next_) {
        channel_next_->LinkBlock(std::move(next_block));
    } else {
        channel_next_ = std::move(next_block);
        links_.SetLink(LinkType::ChannelNext, channel_next_);
    }
}

uint64_t ChannelBlock::GetSize() const {
    return GetBaseSize() + 1 + 1 + 1 + 1 + 4 + 4 + 4 + 4 + 1 + 1 + 2 + 8 + 8 + 8 + 8 + 8 + 8;
}

std::unique_ptr<uint8_t[]> ChannelBlock::Serialize() const {
    auto buffer = SerializeBase();
    uint64_t offset = GetBaseSize();

    std::memcpy(buffer.get() + offset, &type_, sizeof(uint8_t));
    offset += sizeof(uint8_t);

    std::memcpy(buffer.get() + offset, &sync_type_, sizeof(uint8_t));
    offset += sizeof(uint8_t);

    std::memcpy(buffer.get() + offset, &data_type_, sizeof(uint8_t));
    offset += sizeof(uint8_t);

    std::memcpy(buffer.get() + offset, &bit_offset_, sizeof(bit_offset_));
    offset += sizeof(bit_offset_);

    std::memcpy(buffer.get() + offset, &byte_offset_, sizeof(byte_offset_));
    offset += sizeof(byte_offset_);

    std::memcpy(buffer.get() + offset, &bit_count_, sizeof(bit_count_));
    offset += sizeof(bit_count_);

    std::memcpy(buffer.get() + offset, &flags_, sizeof(flags_));
    offset += sizeof(flags_);

    std::memcpy(buffer.get() + offset, &invalidation_bit_pos_, sizeof(invalidation_bit_pos_));
    offset += sizeof(invalidation_bit_pos_);

    std::memcpy(buffer.get() + offset, &precision_, sizeof(precision_));
    offset += sizeof(precision_);

    offset += 1; // reserved_1_

    std::memcpy(buffer.get() + offset, &attachment_count_, sizeof(attachment_count_));
    offset += sizeof(attachment_count_);

    std::memcpy(buffer.get() + offset, &val_range_min_, sizeof(val_range_min_));
    offset += sizeof(val_range_min_);

    std::memcpy(buffer.get() + offset, &val_range_max_, sizeof(val_range_max_));
    offset += sizeof(val_range_max_);

    std::memcpy(buffer.get() + offset, &limit_min_, sizeof(limit_min_));
    offset += sizeof(limit_min_);

    std::memcpy(buffer.get() + offset, &limit_max_, sizeof(limit_max_));
    offset += sizeof(limit_max_);

    std::memcpy(buffer.get() + offset, &limit_ext_min_, sizeof(limit_ext_min_));
    offset += sizeof(limit_ext_min_);

    std::memcpy(buffer.get() + offset, &limit_ext_max_, sizeof(limit_ext_max_));
    offset += sizeof(limit_ext_max_);

    return buffer;
}

size_t ChannelBlock::GetBitCountForType(DataType data_type) {
    switch(data_type) {
        case DataType::UnsignedIntegerLe:
            return sizeof(uint32_t) * 8;
        case DataType::SignedIntegerLe:
            return sizeof(int32_t) * 8;
        case DataType::FloatLe:
            return sizeof(double) * 8;
        default:
            return 0;
    }
}

} // namespace eerie_leap::subsys::mdf::mdf4
