#pragma once

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

#include "subsys/mdf/utilities/block_links.h"
#include "metadata_block.h"
#include "block_base.h"

namespace eerie_leap::subsys::mdf::mdf4 {

using namespace eerie_leap::subsys::mdf::utilities;

class FileHistoryBlock : public BlockBase {
private:
    enum class LinkType: int {
        FileHistoryNext = 0,
        MetadataComment
    };

    BlockLinks<LinkType, 2> links_;

    uint64_t time_ns_;                  // 8 bytes, Absolute time in ns since midnight Jan 1st, 1970
    uint16_t tz_offset_min_;            // 2 bytes, Timezone offset in minutes
    uint16_t dst_offset_min_;           // 2 bytes, Daylight saving time (DST) offset in minutes
    uint8_t time_flags_;                // 1 bytes, Time flags
    // uint8_t reserved_1_[3];          // 3 bytes, Reserved

public:
    FileHistoryBlock();
    virtual ~FileHistoryBlock() = default;

    void SetTimeNs(uint64_t time_ns);

    uint64_t GetBlockSize() const override;
    std::unique_ptr<uint8_t[]> Serialize() const override;
    const IBlockLinks* GetBlockLinks() const override { return &links_; }
    std::vector<std::shared_ptr<ISerializableBlock>> GetChildren() const override {
        return {
            links_.GetLink(LinkType::MetadataComment),
            links_.GetLink(LinkType::FileHistoryNext) };
    }

    void LinkBlock(std::shared_ptr<FileHistoryBlock> next_block);
};

} // namespace eerie_leap::subsys::mdf::mdf4
