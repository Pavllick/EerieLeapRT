#include <algorithm>

#include "subsys/time/time_helpers.hpp"
#include "subsys/mdf/mdf4/channel_block.h"
#include "subsys/mdf/mdf4/channel_group_block.h"
#include "subsys/mdf/mdf4/data_group_block.h"
#include "mdf_helpers.h"

#include "mdf4_file.h"

namespace eerie_leap::subsys::mdf {

using namespace eerie_leap::subsys::time;

Mdf4File::Mdf4File(bool is_finalized): is_finalized_(is_finalized) {
    id_block_ = std::make_unique<mdf4::IdBlock>(is_finalized);

    if(!is_finalized) {
        id_block_->AddStandardFlag(mdf4::IdBlock::StandardFlag::InvalidCGCount);
        id_block_->AddStandardFlag(mdf4::IdBlock::StandardFlag::InvalidLastDTBlock);
        id_block_->AddStandardFlag(mdf4::IdBlock::StandardFlag::InvalidDataVLSDBlock);
    }

    header_block_ = std::make_unique<mdf4::HeaderBlock>();
}

// NOTE: Incorrect start time in Header Block time seems to
// cause asammdf gui to fail parsing the file
void Mdf4File::UpdateCurrentTime(system_clock::time_point time) {
    header_block_->SetCurrentTimeNs(TimeHelpers::ToUint64(time));
}

std::shared_ptr<mdf4::DataGroupBlock> Mdf4File::CreateDataGroup(uint8_t record_id_size_bytes) {
    auto data_group = std::make_shared<mdf4::DataGroupBlock>(record_id_size_bytes);
    data_groups_.emplace(data_group, std::unordered_set<uint64_t>{});

    header_block_->AddDataGroup(data_group);

    return data_group;
}

const std::vector<std::shared_ptr<mdf4::DataGroupBlock>> Mdf4File::GetDataGroups() const {
    std::vector<std::shared_ptr<mdf4::DataGroupBlock>> data_groups;
    for(auto& [data_group, _] : data_groups_)
        data_groups.push_back(data_group);

    return data_groups;
}

std::shared_ptr<mdf4::ChannelGroupBlock> Mdf4File::CreateChannelGroup(mdf4::DataGroupBlock& data_group, uint64_t record_id, const std::string& name) {
    auto data_group_it = std::find_if(
        data_groups_.begin(), data_groups_.end(),
        [&data_group](const auto& pair) {
            return pair.first.get() == &data_group;
        });

    if(data_group_it == data_groups_.end())
        throw std::runtime_error("Data group not found");

    if(data_group_it->second.find(record_id) != data_group_it->second.end())
        throw std::runtime_error("Record ID already exists");

    data_group_it->second.insert(record_id);

    auto channel_group = std::make_shared<mdf4::ChannelGroupBlock>(data_group.GetRecordIdSizeBytes(), record_id, name);
    data_group.AddChannelGroup(channel_group);

    auto channel_data_type = MdfHelpers::ToMdf4ChannelDataType(MdfDataType::Float32);
    auto channel_time = std::make_shared<mdf4::ChannelBlock>(
        mdf4::ChannelBlock::Type::Master,
        mdf4::ChannelBlock::SyncType::Time,
        channel_data_type.data_type,
        channel_data_type.bit_count,
        "timestamp",
        "s");
    channel_group->AddChannel(channel_time);

    return channel_group;
}

std::shared_ptr<mdf4::ChannelBlock> Mdf4File::CreateDataChannel(
    mdf4::ChannelGroupBlock& channel_group, MdfDataType data_type, std::string name, std::string unit) {

    auto channel_data_type = MdfHelpers::ToMdf4ChannelDataType(data_type);
    auto channel = std::make_shared<mdf4::ChannelBlock>(
        mdf4::ChannelBlock::Type::FixedLength,
        mdf4::ChannelBlock::SyncType::NoSync,
        channel_data_type.data_type,
        channel_data_type.bit_count,
        name,
        unit);
    channel_group.AddChannel(channel);

    return channel;
}

mdf4::DataRecord Mdf4File::CreateDataRecord(std::shared_ptr<mdf4::ChannelGroupBlock> channel_group) {
    return mdf4::DataRecord(channel_group);
}

uint64_t Mdf4File::WriteFileToStream(std::streambuf& stream) const {
    auto current_address = id_block_->ResolveAddress(0);
    header_block_->ResolveAddress(current_address);

    auto bytes_written = id_block_->WriteToStream(stream);
    bytes_written += header_block_->WriteToStream(stream);

    return bytes_written;
}

} // namespace eerie_leap::subsys::mdf
