#include "data_group_block.h"

namespace eerie_leap::subsys::mdf::mdf4 {

DataGroupBlock::DataGroupBlock(uint8_t record_id_size_bytes)
    : BlockBase("DG"), record_id_size_bytes_(record_id_size_bytes) {

    data_ = std::make_shared<DataBlock>();
    links_.SetLink(LinkType::Data, data_);
}

uint8_t DataGroupBlock::GetRecordIdSizeBytes() const {
    return record_id_size_bytes_;
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
