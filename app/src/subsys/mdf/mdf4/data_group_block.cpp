#include "data_group_block.h"

namespace eerie_leap::subsys::mdf::mdf4 {

DataGroupBlock::DataGroupBlock(): BlockBase("DG") {
    record_id_size_ = 0;

    data_ = std::make_shared<DataBlock>();
    links_.SetLink(LinkType::Data, data_);
}

void DataGroupBlock::AddChannelGroup(std::shared_ptr<ChannelGroupBlock> channel_group) {
    if(channel_group_) {
        channel_group_->LinkBlock(std::move(channel_group));
    } else {
        channel_group_ = std::move(channel_group);
        links_.SetLink(LinkType::ChannelGroupFirst, channel_group_);
    }
}

void DataGroupBlock::LinkBlock(std::shared_ptr<DataGroupBlock> next_block) {
    if(data_group_next_) {
        data_group_next_->LinkBlock(std::move(next_block));
    } else {
        data_group_next_ = std::move(next_block);
        links_.SetLink(LinkType::DataGroupNext, data_group_next_);
    }
}

uint64_t DataGroupBlock::GetSize() const {
    return GetBaseSize() + 1 + 7;
}

std::unique_ptr<uint8_t[]> DataGroupBlock::Serialize() const {
    auto buffer = SerializeBase();
    uint64_t offset = GetBaseSize();

    std::memcpy(buffer.get() + offset, &record_id_size_, sizeof(record_id_size_));
    offset += sizeof(record_id_size_);

    offset += 7; // reserved_1_

    return buffer;
}


} // namespace eerie_leap::subsys::mdf::mdf4
