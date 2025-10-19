#include "utilities/time/time_helpers.hpp"

#include "subsys/mdf/mdf4/channel_block.h"
#include "subsys/mdf/mdf4/channel_group_block.h"
#include "subsys/mdf/mdf4/data_group_block.h"
#include "mdf_helpers.h"

#include "mdf4_file.h"

namespace eerie_leap::subsys::mdf {

using namespace eerie_leap::utilities::time;

Mdf4File::Mdf4File(bool is_finalized): is_finalized_(is_finalized), record_id_(0) {
    id_block_ = std::make_unique<mdf4::IdBlock>(is_finalized);

    if(!is_finalized) {
        id_block_->AddStandardFlag(mdf4::IdBlock::StandardFlag::InvalidCGCount);
        id_block_->AddStandardFlag(mdf4::IdBlock::StandardFlag::InvalidLastDTBlock);
        id_block_->AddStandardFlag(mdf4::IdBlock::StandardFlag::InvalidDataVLSDBlock);
    }

    header_block_ = std::make_unique<mdf4::HeaderBlock>();
}

void Mdf4File::UpdateCurrentTime(system_clock::time_point time) {
    header_block_->SetCurrentTimeNs(TimeHelpers::ToUint64(time));
}

std::shared_ptr<mdf4::DataGroupBlock> Mdf4File::CreateDataGroup() {
    auto data_group = std::make_shared<mdf4::DataGroupBlock>();
    data_groups_.push_back(data_group);

    header_block_->AddDataGroup(data_group);

    return data_group;
}

const std::vector<std::shared_ptr<mdf4::DataGroupBlock>>& Mdf4File::GetDataGroups() const {
    return data_groups_;
}

std::shared_ptr<mdf4::ChannelGroupBlock> Mdf4File::CreateChannelGroup(mdf4::DataGroupBlock& data_group) {
    auto channel_group = std::make_shared<mdf4::ChannelGroupBlock>(record_id_++);
    data_group.AddChannelGroup(channel_group);

    auto channel_data_type = MdfHelpers::ToMdf4ChannelDataType(MdfDataType::Float32);
    auto channel_time = std::make_shared<mdf4::ChannelBlock>(
        mdf4::ChannelBlock::Type::Master,
        mdf4::ChannelBlock::SyncType::Time,
        channel_data_type.data_type,
        channel_data_type.bit_count,
        "Timestamp",
        "s");
    channel_group->AddChannel(channel_time);

    return channel_group;
}

std::shared_ptr<mdf4::ChannelBlock> Mdf4File::CreateDataChannel(mdf4::ChannelGroupBlock& channel_group, MdfDataType data_type, std::string name, std::string unit) {
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

uint64_t Mdf4File::WriteToStream(std::streambuf& stream) const {
    auto current_address = id_block_->ResolveAddress(0);
    header_block_->ResolveAddress(current_address);

    auto bytes_written = id_block_->WriteToStream(stream);
    bytes_written += header_block_->WriteToStream(stream);

    return bytes_written;
}

} // namespace eerie_leap::subsys::mdf
