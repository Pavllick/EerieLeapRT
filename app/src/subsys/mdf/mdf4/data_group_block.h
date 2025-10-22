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

public:
    DataGroupBlock(uint8_t record_id_size_bytes);
    virtual ~DataGroupBlock() = default;

    uint8_t GetRecordIdSizeBytes() const;

    uint64_t GetBlockSize() const override;
    std::unique_ptr<uint8_t[]> Serialize() const override;
    const IBlockLinks* GetBlockLinks() const override { return &links_; }
    std::vector<std::shared_ptr<ISerializableBlock>> GetChildren() const override {
        return {
            links_.GetLink(LinkType::MetadataComment),
            links_.GetLink(LinkType::ChannelGroupFirst),
            links_.GetLink(LinkType::Data),
            links_.GetLink(LinkType::DataGroupNext) };
    }

    void AddChannelGroup(std::shared_ptr<ChannelGroupBlock> channel_group);
    void LinkBlock(std::shared_ptr<DataGroupBlock> next_block);
};

} // namespace eerie_leap::subsys::mdf::mdf4
