#include "channel_group_block.h"

namespace eerie_leap::subsys::mdf::mdf4 {

ChannelGroupBlock::ChannelGroupBlock(uint64_t record_id): BlockBase("CG") {
    record_id_ = record_id;
    cycle_count_ = 0;
    flags_ = 0;
    path_separator_ = 0;
    data_bytes_ = 0;
    invalidation_bytes_ = 0;
}

void ChannelGroupBlock::AddChannel(std::shared_ptr<ChannelBlock> channel) {
    channel->SetOffsetBytes(data_bytes_);
    data_bytes_ += channel->GetDataBytes();

    if(channel_first_) {
        channel_first_->LinkBlock(std::move(channel));
    } else {
        channel_first_ = std::move(channel);
        links_.SetLink(LinkType::ChannelFirst, channel_first_);
    }
}

void ChannelGroupBlock::LinkBlock(std::shared_ptr<ChannelGroupBlock> next_block) {
    if(channel_group_next_) {
        channel_group_next_->LinkBlock(std::move(next_block));
    } else {
        channel_group_next_ = std::move(next_block);
        links_.SetLink(LinkType::ChannelGroupNext, channel_group_next_);
    }
}

uint64_t ChannelGroupBlock::GetSize() const {
    return GetBaseSize() + 8 + 8 + 2 + 2 + 4 + 4 + 4;
}

std::unique_ptr<uint8_t[]> ChannelGroupBlock::Serialize() const {
    auto buffer = SerializeBase();
    uint64_t offset = GetBaseSize();

    std::memcpy(buffer.get() + offset, &record_id_, sizeof(record_id_));
    offset += sizeof(record_id_);

    std::memcpy(buffer.get() + offset, &cycle_count_, sizeof(cycle_count_));
    offset += sizeof(cycle_count_);

    std::memcpy(buffer.get() + offset, &flags_, sizeof(flags_));
    offset += sizeof(flags_);

    std::memcpy(buffer.get() + offset, &path_separator_, sizeof(path_separator_));
    offset += sizeof(path_separator_);

    offset += 4; // reserved_1_

    std::memcpy(buffer.get() + offset, &data_bytes_, sizeof(data_bytes_));
    offset += sizeof(data_bytes_);

    std::memcpy(buffer.get() + offset, &invalidation_bytes_, sizeof(invalidation_bytes_));
    offset += sizeof(invalidation_bytes_);

    return buffer;
}


} // namespace eerie_leap::subsys::mdf::mdf4
