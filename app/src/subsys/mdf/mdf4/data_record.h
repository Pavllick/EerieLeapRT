#pragma once

#include <cstdint>
#include <memory>
#include <vector>

#include "channel_group_block.h"

namespace eerie_leap::subsys::mdf::mdf4 {

class DataRecord {
private:
    std::shared_ptr<ChannelGroupBlock> channel_group_;

    std::unique_ptr<uint8_t[]> Create(const std::vector<void*>& values) const;

public:
    DataRecord(std::shared_ptr<ChannelGroupBlock> channel_group);
    virtual ~DataRecord() = default;

    uint64_t GetRecordSize() const;
    uint64_t WriteToStream(std::streambuf& stream, const std::vector<void*>& values) const;
};

} // namespace eerie_leap::subsys::mdf::mdf4
