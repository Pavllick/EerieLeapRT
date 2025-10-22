#include "file_history_block.h"

namespace eerie_leap::subsys::mdf::mdf4 {

FileHistoryBlock::FileHistoryBlock(): BlockBase("FH") {
    time_ns_ = 0;
    tz_offset_min_ = 0;
    dst_offset_min_ = 0;
    time_flags_ = 0;

    auto metadata_comment_block = std::make_shared<MetadataBlock>();
    metadata_comment_block->SetText(GetId(),
        "<TX>created</TX>"
        "<tool_id>none</tool_id>"
        "<tool_vendor>Eerie Leap</tool_vendor>"
        "<tool_version>0.0.0</tool_version>"
    );
    links_.SetLink(LinkType::MetadataComment, std::move(metadata_comment_block));
}

void FileHistoryBlock::SetTimeNs(uint64_t time_ns) {
    time_ns_ = time_ns;
}

void FileHistoryBlock::LinkBlock(std::shared_ptr<FileHistoryBlock> next_block) {
    if(links_.GetLink(LinkType::FileHistoryNext)) {
        auto linked_file_history = std::dynamic_pointer_cast<FileHistoryBlock>(links_.GetLink(LinkType::FileHistoryNext));
        linked_file_history->LinkBlock(std::move(next_block));
    } else {
        links_.SetLink(LinkType::FileHistoryNext, std::move(next_block));
    }
}

uint64_t FileHistoryBlock::GetBlockSize() const {
    return GetBaseSize() + 8 + 2 + 2 + 1 + 3;
}

std::unique_ptr<uint8_t[]> FileHistoryBlock::Serialize() const {
    auto buffer = SerializeBase();
    uint64_t offset = GetBaseSize();

    std::memcpy(buffer.get() + offset, &time_ns_, sizeof(time_ns_));
    offset += sizeof(time_ns_);

    std::memcpy(buffer.get() + offset, &tz_offset_min_, sizeof(tz_offset_min_));
    offset += sizeof(tz_offset_min_);

    std::memcpy(buffer.get() + offset, &dst_offset_min_, sizeof(dst_offset_min_));
    offset += sizeof(dst_offset_min_);

    std::memcpy(buffer.get() + offset, &time_flags_, sizeof(time_flags_));
    offset += sizeof(time_flags_);

    offset += 3; // reserved_1_

    return buffer;
}


} // namespace eerie_leap::subsys::mdf::mdf4
