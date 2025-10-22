#include "data_group_block.h"

namespace eerie_leap::subsys::mdf::mdf4 {

DataGroupBlock::DataGroupBlock(uint8_t record_id_size_bytes)
    : BlockBase("DG"), record_id_size_bytes_(record_id_size_bytes) {

    links_.SetLink(LinkType::Data, std::make_shared<DataBlock>());
}

uint8_t DataGroupBlock::GetRecordIdSizeBytes() const {
    return record_id_size_bytes_;
}

void DataGroupBlock::AddChannelGroup(std::shared_ptr<ChannelGroupBlock> channel_group) {
    if(links_.GetLink(LinkType::ChannelGroupFirst)) {
        auto channel_group_first = std::dynamic_pointer_cast<ChannelGroupBlock>(links_.GetLink(LinkType::ChannelGroupFirst));
        channel_group_first->LinkBlock(std::move(channel_group));
    } else {
        links_.SetLink(LinkType::ChannelGroupFirst, channel_group);
    }
}

void DataGroupBlock::LinkBlock(std::shared_ptr<DataGroupBlock> next_block) {
    if(links_.GetLink(LinkType::DataGroupNext)) {
        auto data_group_next = std::dynamic_pointer_cast<DataGroupBlock>(links_.GetLink(LinkType::DataGroupNext));
        data_group_next->LinkBlock(std::move(next_block));
    } else {
        links_.SetLink(LinkType::DataGroupNext, next_block);
    }
}

uint64_t DataGroupBlock::GetBlockSize() const {
    return GetBaseSize() + 1 + 7;
}

std::unique_ptr<uint8_t[]> DataGroupBlock::Serialize() const {
    auto buffer = SerializeBase();
    uint64_t offset = GetBaseSize();

    std::memcpy(buffer.get() + offset, &record_id_size_bytes_, sizeof(record_id_size_bytes_));
    offset += sizeof(record_id_size_bytes_);

    offset += 7; // reserved_1_

    return buffer;
}


} // namespace eerie_leap::subsys::mdf::mdf4
