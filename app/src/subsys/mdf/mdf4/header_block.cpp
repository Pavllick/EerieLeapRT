#include "header_block.h"

namespace eerie_leap::subsys::mdf::mdf4 {

HeaderBlock::HeaderBlock(): BlockBase("HD") {
    start_time_ns_ = 0;
    tz_offset_min_ = 0;
    dst_offset_min_ = 0;
    time_flags_ = 0;
    time_class_ = 0;
    flags_ = 0;
    hd_start_angle_rad_ = 0.0;
    hd_distance_m_ = 0.0;

    links_.SetLink(LinkType::FileHistoryFirst, std::make_shared<FileHistoryBlock>());
}

void HeaderBlock::SetCurrentTimeNs(uint64_t time_ns) {
    start_time_ns_ = time_ns;
    if(links_.GetLink(LinkType::FileHistoryFirst)) {
        auto file_history = std::dynamic_pointer_cast<FileHistoryBlock>(links_.GetLink(LinkType::FileHistoryFirst));
        file_history->SetTimeNs(time_ns);
    }
}

void HeaderBlock::AddFileHistory(std::shared_ptr<FileHistoryBlock> file_history) {
    if(links_.GetLink(LinkType::FileHistoryFirst)) {
        auto linked_file_history = std::dynamic_pointer_cast<FileHistoryBlock>(links_.GetLink(LinkType::FileHistoryFirst));
        linked_file_history->LinkBlock(std::move(file_history));
    }
}

void HeaderBlock::AddDataGroup(std::shared_ptr<DataGroupBlock> data_group) {
    if(links_.GetLink(LinkType::DataGroupFirst)) {
        auto linked_data_group = std::dynamic_pointer_cast<DataGroupBlock>(links_.GetLink(LinkType::DataGroupFirst));
        linked_data_group->LinkBlock(std::move(data_group));
    } else {
        links_.SetLink(LinkType::DataGroupFirst, std::move(data_group));
    }
}

uint64_t HeaderBlock::GetBlockSize() const {
    return GetBaseSize() + 8 + 2 + 2 + 1 + 1 + 1 + 1 + 8 + 8;
}

std::unique_ptr<uint8_t[]> HeaderBlock::Serialize() const {
    auto buffer = SerializeBase();
    uint64_t offset = GetBaseSize();

    std::memcpy(buffer.get() + offset, &start_time_ns_, sizeof(start_time_ns_));
    offset += sizeof(start_time_ns_);

    std::memcpy(buffer.get() + offset, &tz_offset_min_, sizeof(tz_offset_min_));
    offset += sizeof(tz_offset_min_);

    std::memcpy(buffer.get() + offset, &dst_offset_min_, sizeof(dst_offset_min_));
    offset += sizeof(dst_offset_min_);

    std::memcpy(buffer.get() + offset, &time_flags_, sizeof(time_flags_));
    offset += sizeof(time_flags_);

    std::memcpy(buffer.get() + offset, &time_class_, sizeof(time_class_));
    offset += sizeof(time_class_);

    std::memcpy(buffer.get() + offset, &flags_, sizeof(flags_));
    offset += sizeof(flags_);

    offset += 1; // reserved_1_

    std::memcpy(buffer.get() + offset, &hd_start_angle_rad_, sizeof(hd_start_angle_rad_));
    offset += sizeof(hd_start_angle_rad_);

    std::memcpy(buffer.get() + offset, &hd_distance_m_, sizeof(hd_distance_m_));
    offset += sizeof(hd_distance_m_);

    return buffer;
}


} // namespace eerie_leap::subsys::mdf::mdf4
