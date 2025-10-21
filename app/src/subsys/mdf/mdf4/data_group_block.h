#pragma once

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

#include "subsys/mdf/utilities/block_links.h"
#include "channel_group_block.h"
#include "data_block.h"
#include "block_base.h"

namespace eerie_leap::subsys::mdf::mdf4 {

using namespace eerie_leap::subsys::mdf::utilities;

class DataGroupBlock : public BlockBase {
private:
    enum class LinkType: int {
        DataGroupNext = 0,
        ChannelGroupFirst,
        Data,
        MetadataComment
    };

    BlockLinks<LinkType, 4> links_;

    uint8_t record_id_size_bytes_;      // 1 byte, Length of record ID bytes
    // uint8_t reserved_1_[7];          // 7 bytes, Reserved

    std::shared_ptr<DataGroupBlock> data_group_next_;
    std::shared_ptr<ChannelGroupBlock> channel_group_;
    std::shared_ptr<DataBlock> data_;

public:
    DataGroupBlock(uint8_t record_id_size_bytes);
    virtual ~DataGroupBlock() = default;

    uint8_t GetRecordIdSizeBytes() const;

    uint64_t GetBlockSize() const override;
    std::unique_ptr<uint8_t[]> Serialize() const override;
    const IBlockLinks* GetBlockLinks() const override { return &links_; }
    std::vector<std::shared_ptr<ISerializableBlock>> GetChildren() const override {
        return { channel_group_, data_, data_group_next_ };
    }

    void AddChannelGroup(std::shared_ptr<ChannelGroupBlock> channel_group);
    void LinkBlock(std::shared_ptr<DataGroupBlock> next_block);
};

} // namespace eerie_leap::subsys::mdf::mdf4
