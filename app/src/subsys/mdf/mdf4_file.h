#pragma once

#include <memory>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <chrono>

#include "subsys/canbus/can_frame.h"
#include "subsys/mdf/mdf4/channel_group_block.h"
#include "subsys/mdf/mdf4/id_block.h"
#include "subsys/mdf/mdf4/header_block.h"
#include "subsys/mdf/mdf4/data_record.h"
#include "subsys/mdf/mdf4/vlsd_data_record.h"
#include "mdf_data_type.h"

namespace eerie_leap::subsys::mdf {

using namespace std::chrono;
using namespace eerie_leap::subsys::canbus;

class Mdf4File {
private:
    struct CanDataFrameBlocks {
        std::shared_ptr<mdf4::DataRecord> header_data_record;
        std::shared_ptr<mdf4::VlsdDataRecord> raw_data_vlsd_data_record;
    };

    bool is_finalized_;

    std::unique_ptr<mdf4::IdBlock> id_block_;
    std::unique_ptr<mdf4::HeaderBlock> header_block_;
    std::unordered_map<std::shared_ptr<mdf4::DataGroupBlock>, std::unordered_set<uint64_t>> data_groups_;
    std::unordered_map<std::string, std::shared_ptr<mdf4::TextBlock>> text_blocks_;
    std::unordered_map<std::shared_ptr<mdf4::ChannelGroupBlock>, CanDataFrameBlocks> can_data_frame_blocks_;

    std::shared_ptr<mdf4::ChannelBlock> CreateChannelBlock(MdfDataType data_type, std::string name, std::string unit = "");

public:
    static constexpr char* LOG_DATA_FILE_EXTENSION = "mf4";

    Mdf4File(bool is_finalized = true);
    virtual ~Mdf4File() = default;

    void UpdateCurrentTime(system_clock::time_point time);

    std::shared_ptr<mdf4::DataGroupBlock> CreateDataGroup(uint8_t record_id_size_bytes);
    const std::vector<std::shared_ptr<mdf4::DataGroupBlock>> GetDataGroups() const;

    std::shared_ptr<mdf4::ChannelGroupBlock> CreateChannelGroup(std::shared_ptr<mdf4::DataGroupBlock> data_group, uint64_t record_id, const std::string& name);
    std::shared_ptr<mdf4::ChannelGroupBlock> CreateVLSDChannelGroup(std::shared_ptr<mdf4::DataGroupBlock> data_group, uint64_t record_id);
    std::shared_ptr<mdf4::ChannelGroupBlock> CreateCanDataFrameChannelGroup(
        std::shared_ptr<mdf4::DataGroupBlock> data_group,
        std::shared_ptr<mdf4::ChannelGroupBlock> vlsd_channel_group,
        uint64_t record_id,
        const std::string& name);
    std::shared_ptr<mdf4::ChannelBlock> CreateDataChannel(
        std::shared_ptr<mdf4::ChannelGroupBlock> channel_group,
        MdfDataType data_type,
        std::string name,
        std::string unit);
    mdf4::DataRecord CreateDataRecord(std::shared_ptr<mdf4::ChannelGroupBlock> channel_group);
    std::shared_ptr<mdf4::TextBlock> GetOrCreateTextBlock(const std::string& name);

    uint64_t WriteFileToStream(std::streambuf& stream) const;
    uint64_t WriteCanbusDataRecordToStream(
        std::shared_ptr<mdf4::ChannelGroupBlock> channel_group,
        std::streambuf& stream,
        const CanFrame& can_frame,
        float time) const;
};

} // namespace eerie_leap::subsys::mdf
