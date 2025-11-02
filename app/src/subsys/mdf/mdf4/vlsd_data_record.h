#pragma once

#include <cstdint>
#include <memory>
#include <span>
#include <vector>

#include "channel_group_block.h"

namespace eerie_leap::subsys::mdf::mdf4 {

// Variable length signal data record
class VlsdDataRecord {
private:
    std::shared_ptr<ChannelGroupBlock> vlsd_channel_group_;
    uint64_t offset_channel_size_bytes_;
    uint64_t offset_;

    std::unique_ptr<uint8_t[]> Create(const std::span<const uint8_t>& data) const;
    uint64_t GetRecordSizeBytes(int data_size) const;

public:
    VlsdDataRecord(std::shared_ptr<ChannelGroupBlock> vlsd_channel_group, uint64_t offset_channel_size_bytes);
    virtual ~VlsdDataRecord() = default;
    void Reset();
    std::vector<uint8_t> GetOffsetData() const;
    uint64_t WriteToStream(std::streambuf& stream, const std::span<const uint8_t>& data);
};

} // namespace eerie_leap::subsys::mdf::mdf4
