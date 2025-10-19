#pragma once

#include <memory>
#include <vector>
#include <chrono>

#include "subsys/mdf/mdf4/channel_group_block.h"
#include "subsys/mdf/mdf4/id_block.h"
#include "subsys/mdf/mdf4/header_block.h"

namespace eerie_leap::subsys::mdf {

using namespace std::chrono;

class Mdf4File {
private:
    bool is_finalized_;
    uint64_t record_id_;

    std::unique_ptr<mdf4::IdBlock> id_block_;
    std::unique_ptr<mdf4::HeaderBlock> header_block_;
    std::vector<std::shared_ptr<mdf4::DataGroupBlock>> data_groups_;

public:
    Mdf4File(bool is_finalized = true);
    virtual ~Mdf4File() = default;

    void UpdateCurrentTime(system_clock::time_point time);

    std::shared_ptr<mdf4::DataGroupBlock> CreateDataGroup();
    const std::vector<std::shared_ptr<mdf4::DataGroupBlock>>& GetDataGroups() const;

    std::shared_ptr<mdf4::ChannelGroupBlock> CreateChannelGroup(mdf4::DataGroupBlock& data_group);
    std::shared_ptr<mdf4::ChannelBlock> CreateDataChannel(mdf4::ChannelGroupBlock& channel_group, mdf4::ChannelBlock::DataType data_type, std::string name, std::string unit);

    uint64_t WriteToStream(std::streambuf& stream) const;
};

} // namespace eerie_leap::subsys::mdf
