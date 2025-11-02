#include <set>

#include "text_block.h"

#include "channel_group_block.h"

namespace eerie_leap::subsys::mdf::mdf4 {

ChannelGroupBlock::ChannelGroupBlock(uint8_t record_id_size_bytes, uint64_t record_id)
    : BlockBase("CG"), record_id_size_bytes_(record_id_size_bytes), record_id_(record_id) {

    if(std::set<uint8_t>{0, 1, 2, 4, 8}.count(record_id_size_bytes_) == 0)
        throw std::runtime_error("Invalid record ID size bytes");

    cycle_count_ = 0;
    flags_ = 0;
    path_separator_ = 0;
    data_bytes_ = 0;
    invalidation_bytes_ = 0;
}

uint16_t ChannelGroupBlock::GetFlags() const {
    return flags_;
}

uint64_t ChannelGroupBlock::GetRecordId() const {
    return record_id_;
}

uint8_t ChannelGroupBlock::GetRecordIdSizeBytes() const {
    return record_id_size_bytes_;
}

std::vector<uint8_t> ChannelGroupBlock::GetRecordIdData() const {
    uint8_t id_size_bytes = GetRecordIdSizeBytes();
    std::vector<uint8_t> buffer(id_size_bytes);

    if(id_size_bytes == 0) {

    } else if(id_size_bytes == 1) {
        uint8_t id = static_cast<uint8_t>(GetRecordId());
        std::memcpy(buffer.data(), &id, id_size_bytes);
    } else if(id_size_bytes == 2) {
        uint16_t id = static_cast<uint16_t>(GetRecordId());
        std::memcpy(buffer.data(), &id, id_size_bytes);
    } else if(id_size_bytes == 4) {
        uint32_t id = static_cast<uint32_t>(GetRecordId());
        std::memcpy(buffer.data(), &id, id_size_bytes);
    } else if(id_size_bytes == 8) {
        uint64_t id = static_cast<uint64_t>(GetRecordId());
        std::memcpy(buffer.data(), &id, id_size_bytes);
    } else {
        throw std::runtime_error("Invalid record ID size bytes");
    }

    return buffer;
}

uint32_t ChannelGroupBlock::GetDataSizeBytes() const {
    return data_bytes_;
}

std::vector<std::shared_ptr<ChannelBlock>> ChannelGroupBlock::GetChannels() const {
    std::vector<std::shared_ptr<ChannelBlock>> channels;

    auto next_channel = std::dynamic_pointer_cast<ChannelBlock>(links_.GetLink(LinkType::ChannelFirst));
    while(next_channel) {
        channels.push_back(next_channel);
        next_channel = next_channel->GetLinkedChannel();
    }

    return channels;
}

void ChannelGroupBlock::AddChannel(std::shared_ptr<ChannelBlock> channel) {
    channel->SetOffsetBytes(data_bytes_);
    data_bytes_ += channel->GetDataSizeBytes();

    if(links_.GetLink(LinkType::ChannelFirst)) {
        auto first_channel = std::dynamic_pointer_cast<ChannelBlock>(links_.GetLink(LinkType::ChannelFirst));
        first_channel->LinkBlock(std::move(channel));
    } else {
        links_.SetLink(LinkType::ChannelFirst, channel);
    }
}

void ChannelGroupBlock::LinkBlock(std::shared_ptr<ChannelGroupBlock> next_block) {
    if(links_.GetLink(LinkType::ChannelGroupNext)) {
        auto next_channel_group = dynamic_pointer_cast<ChannelGroupBlock>(links_.GetLink(LinkType::ChannelGroupNext));
        next_channel_group->LinkBlock(std::move(next_block));
    } else {
        links_.SetLink(LinkType::ChannelGroupNext, next_block);
    }
}

void ChannelGroupBlock::AddSourceInformation(std::shared_ptr<SourceInformationBlock> source_information) {
    links_.SetLink(LinkType::SourceInformation, source_information);
}

void ChannelGroupBlock::SetFlags(uint16_t flags) {
    flags_ = flags;
}

void ChannelGroupBlock::SetPathSeparator(uint16_t path_separator) {
    path_separator_ = path_separator;
}

void ChannelGroupBlock::SetName(std::shared_ptr<TextBlock> name) {
    links_.SetLink(LinkType::TextAcquisitionName, name);
}

uint64_t ChannelGroupBlock::GetBlockSize() const {
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
