#pragma once

#include <memory>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <chrono>

#include "subsys/mdf/mdf4/channel_group_block.h"
#include "subsys/mdf/mdf4/id_block.h"
#include "subsys/mdf/mdf4/header_block.h"
#include "subsys/mdf/mdf4/data_record.h"
#include "mdf_data_type.h"

namespace eerie_leap::subsys::mdf {

using namespace std::chrono;

class Mdf4File {
private:
    bool is_finalized_;

    std::unique_ptr<mdf4::IdBlock> id_block_;
    std::unique_ptr<mdf4::HeaderBlock> header_block_;
    std::unordered_map<std::shared_ptr<mdf4::DataGroupBlock>, std::unordered_set<uint64_t>> data_groups_;

public:
    static constexpr char* LOG_DATA_FILE_EXTENSION = "mf4";

    Mdf4File(bool is_finalized = true);
    virtual ~Mdf4File() = default;

    void UpdateCurrentTime(system_clock::time_point time);

    std::shared_ptr<mdf4::DataGroupBlock> CreateDataGroup(uint8_t record_id_size_bytes);
    const std::vector<std::shared_ptr<mdf4::DataGroupBlock>> GetDataGroups() const;

    std::shared_ptr<mdf4::ChannelGroupBlock> CreateChannelGroup(mdf4::DataGroupBlock& data_group, uint64_t record_id, const std::string& name);
    std::shared_ptr<mdf4::ChannelBlock> CreateDataChannel(
        mdf4::ChannelGroupBlock& channel_group, MdfDataType data_type, std::string name, std::string unit);
    mdf4::DataRecord CreateDataRecord(std::shared_ptr<mdf4::ChannelGroupBlock> channel_group);

    uint64_t WriteFileToStream(std::streambuf& stream) const;
};

} // namespace eerie_leap::subsys::mdf
