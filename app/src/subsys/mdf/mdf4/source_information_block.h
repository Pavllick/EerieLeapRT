#pragma once

#include <cstdint>
#include <memory>
#include <string>

#include "subsys/mdf/utilities/block_links.h"
#include "block_base.h"
#include "text_block.h"

namespace eerie_leap::subsys::mdf::mdf4 {

using namespace eerie_leap::subsys::mdf::utilities;

class SourceInformationBlock : public BlockBase {
public:
    enum class SourceType : uint8_t {
        Other = 0,
        Ecu = 1,
        Bus = 2,
        IoDevice = 3,
        Tool = 4,
        User = 5
    };

    enum class BusType : uint8_t {
        None = 0,
        Other = 1,
        Can = 2,
        Lin = 3,
        Most = 4,
        FlexRay = 5,
        Kline = 6,
        Ethernet = 7,
        Usb = 8
    };

    enum class Flag: uint8_t {
        Default = 0x00,
        Simulated = 0x01,
    };

private:
    enum class LinkType: int {
        TextName = 0,
        TextPath,
        MetadataComment
    };

    BlockLinks<LinkType, 3> links_;

    SourceType source_type_;        // 1 bytes, Source type
    BusType bus_type_;              // 1 bytes, Bus type
    uint8_t flags_;                 // 1 bytes, Flags
    // uint8_t reserved_1_[5];      // 5 bytes, Reserved

public:
    SourceInformationBlock(SourceType source_type, BusType bus_type);
    virtual ~SourceInformationBlock() = default;

    uint64_t GetBlockSize() const override;
    std::unique_ptr<uint8_t[]> Serialize() const override;
    const IBlockLinks* GetBlockLinks() const override { return &links_; }
    std::vector<std::shared_ptr<ISerializableBlock>> GetChildren() const override {
        return {
            links_.GetLink(LinkType::TextName),
            links_.GetLink(LinkType::TextPath),
            links_.GetLink(LinkType::MetadataComment) };
    }

    void SetName(std::shared_ptr<TextBlock> name);
    void SetPath(std::shared_ptr<TextBlock> path);
};

} // namespace eerie_leap::subsys::mdf::mdf4
