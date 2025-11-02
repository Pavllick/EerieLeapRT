#include <cstdint>
#include <memory>

#include "data_record.h"

namespace eerie_leap::subsys::mdf::mdf4 {

DataRecord::DataRecord(std::shared_ptr<ChannelGroupBlock> channel_group)
    : channel_group_(std::move(channel_group)) { }

uint64_t DataRecord::GetRecordSizeBytes() const {
    return channel_group_->GetRecordIdSizeBytes() + channel_group_->GetDataSizeBytes();
}

std::unique_ptr<uint8_t[]> DataRecord::Create(const std::vector<void*>& values) const {
    auto size = GetRecordSizeBytes();
    auto buffer = std::make_unique<uint8_t[]>(size);

    auto id_data_bytes = channel_group_->GetRecordIdData();
    std::memcpy(buffer.get(), id_data_bytes.data(), id_data_bytes.size());
    uint32_t id_offset = id_data_bytes.size();

    auto channels = channel_group_->GetChannels();
    if(channels.size() != values.size())
        throw std::runtime_error("Invalid number of values");

    for(int i = 0; i < channels.size(); i++) {
        uint32_t offset = id_offset + channels[i]->GetDataOffsetBytes();
        std::memcpy(buffer.get() + offset, values[i], channels[i]->GetDataSizeBytes());
    }

    return buffer;
}

uint64_t DataRecord::WriteToStream(std::streambuf& stream, const std::vector<void*>& values) const {
    const auto record_data = Create(values);

    auto ret = stream.sputn(
        reinterpret_cast<const char*>(record_data.get()),
        static_cast<std::streamsize>(GetRecordSizeBytes()));

    if(ret != GetRecordSizeBytes())
        throw std::ios_base::failure("End of stream reached (EOF).");

    return ret;
}

uint64_t DataRecord::WriteToStream(std::streambuf& stream, const std::span<uint8_t>& data) const {
    auto id_data_bytes = channel_group_->GetRecordIdData();

    auto ret = stream.sputn(
        reinterpret_cast<const char*>(id_data_bytes.data()),
        static_cast<std::streamsize>(id_data_bytes.size()));

    if(ret != id_data_bytes.size())
        throw std::ios_base::failure("End of stream reached (EOF).");

    ret = stream.sputn(
        reinterpret_cast<const char*>(data.data()),
        static_cast<std::streamsize>(data.size()));

    if(ret != data.size())
        throw std::ios_base::failure("End of stream reached (EOF).");

    return ret;
}

} // namespace eerie_leap::subsys::mdf::mdf4
