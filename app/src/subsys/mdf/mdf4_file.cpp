#include <algorithm>
#include <utility>

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

std::shared_ptr<mdf4::ChannelGroupBlock> Mdf4File::CreateChannelGroup(std::shared_ptr<mdf4::DataGroupBlock> data_group, uint64_t record_id, const std::string& name) {
    if(!data_groups_.contains(data_group))
        throw std::runtime_error("Data group not found");

    if(data_groups_.at(data_group).find(record_id) != data_groups_.at(data_group).end())
        throw std::runtime_error("Record ID already exists");

    data_groups_.at(data_group).insert(record_id);

    auto channel_group = std::make_shared<mdf4::ChannelGroupBlock>(
        data_group->GetRecordIdSizeBytes(),
        record_id);
    channel_group->SetName(GetOrCreateTextBlock(name));
    data_group->AddChannelGroup(channel_group);

    auto channel_data_type = MdfHelpers::ToMdf4ChannelDataType(MdfDataType::Float32);
    auto channel_time = std::make_shared<mdf4::ChannelBlock>(
        mdf4::ChannelBlock::Type::Master,
        mdf4::ChannelBlock::SyncType::Time,
        channel_data_type.data_type,
        channel_data_type.bit_count);
    channel_time->SetName(GetOrCreateTextBlock("Timestamp"));
    channel_time->SetUnit(GetOrCreateTextBlock("s"));
    channel_group->AddChannel(channel_time);

    return channel_group;
}

std::shared_ptr<mdf4::ChannelGroupBlock> Mdf4File::CreateVLSDChannelGroup(std::shared_ptr<mdf4::DataGroupBlock> data_group, uint64_t record_id) {
    if(!data_groups_.contains(data_group))
        throw std::runtime_error("Data group not found");

    if(data_groups_.at(data_group).find(record_id) != data_groups_.at(data_group).end())
        throw std::runtime_error("Record ID already exists");

    data_groups_.at(data_group).insert(record_id);

    auto channel_group = std::make_shared<mdf4::ChannelGroupBlock>(
        data_group->GetRecordIdSizeBytes(),
        record_id);
    channel_group->SetFlags(std::to_underlying(mdf4::ChannelGroupBlock::Flag::VlsdChannel));
    data_group->AddChannelGroup(channel_group);

    return channel_group;
}

std::shared_ptr<mdf4::ChannelGroupBlock> Mdf4File::CreateCanDataFrameChannelGroup(
    std::shared_ptr<mdf4::DataGroupBlock> data_group,
    std::shared_ptr<mdf4::ChannelGroupBlock> vlsd_channel_group,
    uint64_t record_id,
    const std::string& name) {

    if(!(vlsd_channel_group->GetFlags() & std::to_underlying(mdf4::ChannelGroupBlock::Flag::VlsdChannel)))
        throw std::runtime_error("Invalid channel group flags");

    auto channel_group = CreateChannelGroup(data_group, record_id, name);
    channel_group->SetFlags(
        std::to_underlying(mdf4::ChannelGroupBlock::Flag::BusEvent)
        | std::to_underlying(mdf4::ChannelGroupBlock::Flag::PlainBusEvent));
    channel_group->SetPathSeparator('.');

    auto source_information = std::make_shared<mdf4::SourceInformationBlock>(
        mdf4::SourceInformationBlock::SourceType::Bus,
        mdf4::SourceInformationBlock::BusType::Can);
    source_information->SetName(GetOrCreateTextBlock("CAN"));
    source_information->SetPath(GetOrCreateTextBlock("CAN"));
    channel_group->AddSourceInformation(source_information);

    auto can_data_frame_channel = CreateChannelBlock(MdfDataType::ByteArray, "CAN_DataFrame");
    can_data_frame_channel->SetFlags(std::to_underlying(mdf4::ChannelBlock::Flag::BusEvent));
    can_data_frame_channel->SetBitCount(80);
    channel_group->AddChannel(can_data_frame_channel);

    auto can_data_frame_bus_channel = CreateChannelBlock(MdfDataType::Uint32, "CAN_DataFrame.BusChannel");
    can_data_frame_bus_channel->SetFlags(std::to_underlying(mdf4::ChannelBlock::Flag::BusEvent));
    can_data_frame_bus_channel->SetOffsetBytes(4);
    can_data_frame_bus_channel->SetBitCount(2);
    can_data_frame_channel->SetArrayBlock(can_data_frame_bus_channel);

    auto can_data_frame_id_channel = CreateChannelBlock(MdfDataType::Uint32, "CAN_DataFrame.ID");
    can_data_frame_id_channel->SetFlags(std::to_underlying(mdf4::ChannelBlock::Flag::BusEvent));
    can_data_frame_id_channel->SetOffsetBytes(4);
    can_data_frame_id_channel->SetOffsetBits(2);
    can_data_frame_id_channel->SetBitCount(29);
    can_data_frame_bus_channel->LinkBlock(can_data_frame_id_channel);

    // IDE (Identifier Extension) | 0 - 11 bit ID, 1 - 29 bit ID
    auto can_data_frame_ide_channel = CreateChannelBlock(MdfDataType::Uint32, "CAN_DataFrame.IDE");
    can_data_frame_ide_channel->SetFlags(std::to_underlying(mdf4::ChannelBlock::Flag::BusEvent));
    can_data_frame_ide_channel->SetOffsetBytes(7);
    can_data_frame_ide_channel->SetOffsetBits(7);
    can_data_frame_ide_channel->SetBitCount(1);
    can_data_frame_bus_channel->LinkBlock(can_data_frame_ide_channel);

    // Dir (Direction) | 0 - Receive, 1 - Transmit
    auto can_data_frame_dir_channel = CreateChannelBlock(MdfDataType::Uint32, "CAN_DataFrame.Dir");
    can_data_frame_dir_channel->SetFlags(std::to_underlying(mdf4::ChannelBlock::Flag::BusEvent));
    can_data_frame_dir_channel->SetOffsetBytes(8);
    can_data_frame_dir_channel->SetBitCount(1);
    can_data_frame_bus_channel->LinkBlock(can_data_frame_dir_channel);

    auto can_data_frame_data_length_channel = CreateChannelBlock(MdfDataType::Uint32, "CAN_DataFrame.DataLength");
    can_data_frame_data_length_channel->SetFlags(std::to_underlying(mdf4::ChannelBlock::Flag::BusEvent));
    can_data_frame_data_length_channel->SetOffsetBytes(8);
    can_data_frame_data_length_channel->SetOffsetBits(1);
    can_data_frame_data_length_channel->SetBitCount(7);
    can_data_frame_bus_channel->LinkBlock(can_data_frame_data_length_channel);

    // EDL (Extended Data Length) | 0 - Standard CAN, 1 - CAN FD
    auto can_data_frame_edl_channel = CreateChannelBlock(MdfDataType::Uint32, "CAN_DataFrame.EDL");
    can_data_frame_edl_channel->SetFlags(std::to_underlying(mdf4::ChannelBlock::Flag::BusEvent));
    can_data_frame_edl_channel->SetOffsetBytes(9);
    can_data_frame_edl_channel->SetBitCount(1);
    can_data_frame_bus_channel->LinkBlock(can_data_frame_edl_channel);

    // BRS (Bit Rate Switch)
    auto can_data_frame_brs_channel = CreateChannelBlock(MdfDataType::Uint32, "CAN_DataFrame.BRS");
    can_data_frame_brs_channel->SetFlags(std::to_underlying(mdf4::ChannelBlock::Flag::BusEvent));
    can_data_frame_brs_channel->SetOffsetBytes(9);
    can_data_frame_brs_channel->SetOffsetBits(1);
    can_data_frame_brs_channel->SetBitCount(1);
    can_data_frame_bus_channel->LinkBlock(can_data_frame_brs_channel);

    // DLC (Data Length Code)
    auto can_data_frame_dlc_channel = CreateChannelBlock(MdfDataType::Uint32, "CAN_DataFrame.DLC");
    can_data_frame_dlc_channel->SetFlags(std::to_underlying(mdf4::ChannelBlock::Flag::BusEvent));
    can_data_frame_dlc_channel->SetOffsetBytes(9);
    can_data_frame_dlc_channel->SetOffsetBits(2);
    can_data_frame_dlc_channel->SetBitCount(4);
    can_data_frame_bus_channel->LinkBlock(can_data_frame_dlc_channel);

    // VLSD data block offset
    auto can_data_frame_data_bytes_channel = CreateChannelBlock(MdfDataType::ByteArray, "CAN_DataFrame.DataBytes");
    can_data_frame_data_bytes_channel->SetType(mdf4::ChannelBlock::Type::VariableLength);
    can_data_frame_data_bytes_channel->SetFlags(std::to_underlying(mdf4::ChannelBlock::Flag::BusEvent));
    can_data_frame_data_bytes_channel->SetOffsetBytes(10);
    can_data_frame_data_bytes_channel->SetBitCount(32);
    can_data_frame_data_bytes_channel->SetSignalDataBlock(vlsd_channel_group);
    can_data_frame_bus_channel->LinkBlock(can_data_frame_data_bytes_channel);

    CanDataFrameBlocks can_data_frame_blocks = {
        .header_data_record = std::make_shared<mdf4::DataRecord>(channel_group),
        .raw_data_vlsd_data_record = std::make_shared<mdf4::VlsdDataRecord>(
            vlsd_channel_group,
            can_data_frame_data_bytes_channel->GetDataSizeBytes()
        )
    };
    can_data_frame_blocks_.emplace(channel_group, can_data_frame_blocks);

    return channel_group;
}

std::shared_ptr<mdf4::ChannelBlock> Mdf4File::CreateChannelBlock(MdfDataType data_type, std::string name, std::string unit) {
    auto channel_data_type = MdfHelpers::ToMdf4ChannelDataType(data_type);
    auto channel = std::make_shared<mdf4::ChannelBlock>(
        mdf4::ChannelBlock::Type::FixedLength,
        mdf4::ChannelBlock::SyncType::NoSync,
        channel_data_type.data_type,
        channel_data_type.bit_count);
    if(!name.empty())
        channel->SetName(GetOrCreateTextBlock(name));
    if(!unit.empty())
        channel->SetUnit(GetOrCreateTextBlock(unit));

    return channel;
}

std::shared_ptr<mdf4::ChannelBlock> Mdf4File::CreateDataChannel(
    std::shared_ptr<mdf4::ChannelGroupBlock> channel_group, MdfDataType data_type, std::string name, std::string unit) {

    auto channel = CreateChannelBlock(data_type, name, unit);
    channel_group->AddChannel(channel);

    return channel;
}

mdf4::DataRecord Mdf4File::CreateDataRecord(std::shared_ptr<mdf4::ChannelGroupBlock> channel_group) {
    return mdf4::DataRecord(channel_group);
}

std::shared_ptr<mdf4::TextBlock> Mdf4File::GetOrCreateTextBlock(const std::string& name) {
    if(!text_blocks_.contains(name)) {
        auto text_block = std::make_shared<mdf4::TextBlock>();
        text_block->SetText(name);
        text_blocks_.emplace(name, text_block);
    }

    return text_blocks_[name];
}

uint64_t Mdf4File::WriteFileToStream(std::streambuf& stream) const {
    for(auto& [_, can_data_frame_block] : can_data_frame_blocks_)
        can_data_frame_block.raw_data_vlsd_data_record->Reset();

    id_block_->Reset();
    auto current_address = id_block_->ResolveAddress(0);
    header_block_->Reset();
    header_block_->ResolveAddress(current_address);

    auto bytes_written = id_block_->WriteToStream(stream);
    bytes_written += header_block_->WriteToStream(stream);

    return bytes_written;
}

uint64_t Mdf4File::WriteCanbusDataRecordToStream(
        std::shared_ptr<mdf4::ChannelGroupBlock> channel_group,
        std::streambuf& stream,
        const CanFrame& can_frame,
        float time) const {

    if(!can_data_frame_blocks_.contains(channel_group))
        throw std::runtime_error("Invalid channel group");

    auto& can_data_frame_block = can_data_frame_blocks_.at(channel_group);

    std::vector<uint8_t> data(channel_group->GetDataSizeBytes());
    memset(data.data(), 0, data.size());

    int offset = 0;

    // Timestamp
    std::memcpy(data.data() + offset, &time, sizeof(float));
    offset += sizeof(float);

    uint32_t data_pack_0 = 0;

    // CAN_DataFrame.BusChannel
    // 2 bits
    uint32_t bus_channel = 0;
    data_pack_0 |= bus_channel;

    // CAN_DataFrame.ID
    // 29 bits
    uint32_t frame_id = can_frame.id;
    data_pack_0 |= frame_id  << 2;

    // CAN_DataFrame.IDE
    // 1 bit
    uint32_t frame_ide = 0;
    data_pack_0 |= frame_ide << 31;

    std::memcpy(data.data() + offset, &data_pack_0, sizeof(data_pack_0));
    offset += sizeof(data_pack_0);

    uint8_t data_pack_1 = 0;

    // CAN_DataFrame.Dir
    // 1 bit
    uint8_t frame_dir = can_frame.is_transmit ? 1 : 0;
    data_pack_1 |= frame_dir;

    // CAN_DataFrame.DataLength
    // 7 bits
    uint8_t frame_data_length = can_frame.data.size();
    data_pack_1 |= frame_data_length << 1;

    std::memcpy(data.data() + offset, &data_pack_1, sizeof(data_pack_1));
    offset += sizeof(data_pack_1);

    uint8_t data_pack_2 = 0;

    // CAN_DataFrame.EDL
    // 1 bit
    uint8_t frame_edl = can_frame.is_can_fd ? 1 : 0;
    data_pack_2 |= frame_edl;

    // CAN_DataFrame.BRS
    // 1 bit
    uint8_t frame_brs = 0;
    data_pack_2 |= frame_brs << 1;

    // CAN_DataFrame.DLC
    // 4 bits
    uint8_t frame_dlc = can_frame.data.size();
    data_pack_2 |= frame_dlc << 2;

    std::memcpy(data.data() + offset, &data_pack_2, sizeof(data_pack_2));
    offset += sizeof(data_pack_2);

    // CAN_DataFrame.DataBytes
    auto data_pack_3 = can_data_frame_block.raw_data_vlsd_data_record->GetOffsetData();
    std::memcpy(data.data() + offset, data_pack_3.data(), data_pack_3.size());

    auto bytes_written = can_data_frame_block.header_data_record->WriteToStream(stream, data);
    bytes_written += can_data_frame_block.raw_data_vlsd_data_record->WriteToStream(stream, can_frame.data);

    return bytes_written;
}

} // namespace eerie_leap::subsys::mdf
