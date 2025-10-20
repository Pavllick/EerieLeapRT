#include <cstdint>
#include <memory>

#include "data_record.h"

namespace eerie_leap::subsys::mdf::mdf4 {

DataRecord::DataRecord(std::shared_ptr<ChannelGroupBlock> channel_group)
    : channel_group_(std::move(channel_group)) { }

uint64_t DataRecord::GetRecordSize() const {
    return channel_group_->GetRecordIdSizeBytes() + channel_group_->GetDataSizeBytes();
}

std::unique_ptr<uint8_t[]> DataRecord::Create(const std::vector<void*>& values) const {
    auto size = GetRecordSize();
    auto buffer = std::make_unique<uint8_t[]>(size);

    uint8_t id_size_bytes = channel_group_->GetRecordIdSizeBytes();
    if(id_size_bytes == 0) {

    } else if(id_size_bytes == 1) {
        uint8_t id = static_cast<uint8_t>(channel_group_->GetRecordId());
        std::memcpy(buffer.get(), &id, id_size_bytes);
    } else if(id_size_bytes == 2) {
        uint16_t id = static_cast<uint16_t>(channel_group_->GetRecordId());
        std::memcpy(buffer.get(), &id, id_size_bytes);
    } else if(id_size_bytes == 4) {
        uint32_t id = static_cast<uint32_t>(channel_group_->GetRecordId());
        std::memcpy(buffer.get(), &id, id_size_bytes);
    } else if(id_size_bytes == 8) {
        uint64_t id = static_cast<uint64_t>(channel_group_->GetRecordId());
        std::memcpy(buffer.get(), &id, id_size_bytes);
    } else {
        throw std::runtime_error("Invalid record ID size bytes");
    }
    uint32_t id_offset = id_size_bytes;

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
        static_cast<std::streamsize>(GetRecordSize()));

    if(ret != GetRecordSize())
        throw std::ios_base::failure("End of stream reached (EOF).");

    return ret;
}

} // namespace eerie_leap::subsys::mdf::mdf4
