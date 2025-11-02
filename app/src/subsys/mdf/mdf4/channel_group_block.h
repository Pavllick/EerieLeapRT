#pragma once

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

#include "subsys/mdf/utilities/block_links.h"
#include "metadata_block.h"
#include "channel_block.h"
#include "source_information_block.h"
#include "block_base.h"

namespace eerie_leap::subsys::mdf::mdf4 {

using namespace eerie_leap::subsys::mdf::utilities;

class ChannelGroupBlock : public BlockBase {
public:
    enum class Flag: uint16_t {
        Default = 0x0000,
        VlsdChannel = 0x0001,
        BusEvent = 0x0002,
        PlainBusEvent = 0x0004,
        RemoteMaster = 0x0008,
        EventSignal = 0x00010
    };

private:
    enum class LinkType: int {
        ChannelGroupNext = 0,
        ChannelFirst,
        TextAcquisitionName,
        SourceInformation,
        SampleReductionFirst,
        MetadataComment
    };

    BlockLinks<LinkType, 6> links_;

    uint64_t record_id_;                 // 8 bytes, Record ID
    uint64_t cycle_count_;               // 8 bytes, Number of cycles
    uint16_t flags_;                     // 2 bytes, Flags
    uint16_t path_separator_;            // 2 bytes, Path separator
    // uint8_t reserved_1_[4];           // 4 bytes, Reserved
    uint32_t data_bytes_;                // 4 bytes, Number of bytes in record used for sample values
    uint32_t invalidation_bytes_;        // 4 bytes, Number of bytes in record used for invalidation bits

    uint8_t record_id_size_bytes_;

public:
    ChannelGroupBlock(uint8_t record_id_size_bytes, uint64_t record_id);
    virtual ~ChannelGroupBlock() = default;

    uint16_t GetFlags() const;
    uint64_t GetRecordId() const;
    uint8_t GetRecordIdSizeBytes() const;
    std::vector<uint8_t> GetRecordIdData() const;
    uint32_t GetDataSizeBytes() const;
    std::vector<std::shared_ptr<ChannelBlock>> GetChannels() const;

    uint64_t GetBlockSize() const override;
    std::unique_ptr<uint8_t[]> Serialize() const override;
    const IBlockLinks* GetBlockLinks() const override { return &links_; }
    std::vector<std::shared_ptr<ISerializableBlock>> GetChildren() const override {
        return {
            links_.GetLink(LinkType::MetadataComment),
            links_.GetLink(LinkType::TextAcquisitionName),
            links_.GetLink(LinkType::SourceInformation),
            links_.GetLink(LinkType::SampleReductionFirst),
            links_.GetLink(LinkType::ChannelFirst),
            links_.GetLink(LinkType::ChannelGroupNext) };
    }

    void AddChannel(std::shared_ptr<ChannelBlock> channel);
    void LinkBlock(std::shared_ptr<ChannelGroupBlock> next_block);
    void AddSourceInformation(std::shared_ptr<SourceInformationBlock> source_information);
    void SetFlags(uint16_t flags);
    void SetPathSeparator(uint16_t path_separator);
    void SetName(std::shared_ptr<TextBlock> name);
};

} // namespace eerie_leap::subsys::mdf::mdf4
