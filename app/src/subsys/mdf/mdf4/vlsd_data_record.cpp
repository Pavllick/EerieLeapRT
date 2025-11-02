#include <cstdint>
#include <memory>

#include "vlsd_data_record.h"

namespace eerie_leap::subsys::mdf::mdf4 {

VlsdDataRecord::VlsdDataRecord(std::shared_ptr<ChannelGroupBlock> vlsd_channel_group, uint64_t offset_channel_size_bytes)
    : vlsd_channel_group_(std::move(vlsd_channel_group))
    , offset_channel_size_bytes_(offset_channel_size_bytes),
    offset_(0) {

    if(!(vlsd_channel_group_->GetFlags() & std::to_underlying(ChannelGroupBlock::Flag::VlsdChannel)))
        throw std::runtime_error("Invalid channel group flags");
}

void VlsdDataRecord::Reset() {
    offset_ = 0;
}

uint64_t VlsdDataRecord::GetRecordSizeBytes(int data_size) const {
    return vlsd_channel_group_->GetRecordIdSizeBytes()
    + 4 // data length
    + data_size;
}

std::unique_ptr<uint8_t[]> VlsdDataRecord::Create(const std::span<const uint8_t>& data) const {
    auto size = GetRecordSizeBytes(data.size());
    auto buffer = std::make_unique<uint8_t[]>(size);

    auto id_data_bytes = vlsd_channel_group_->GetRecordIdData();
    std::memcpy(buffer.get(), id_data_bytes.data(), id_data_bytes.size());
    uint32_t offset = id_data_bytes.size();

    uint32_t data_length = data.size();
    std::memcpy(buffer.get() + offset, &data_length, sizeof(uint32_t));
    offset += sizeof(uint32_t);

    std::memcpy(buffer.get() + offset, data.data(), data.size());

    return buffer;
}

std::vector<uint8_t> VlsdDataRecord::GetOffsetData() const {
    std::vector<uint8_t> buffer(offset_channel_size_bytes_);

    if(offset_channel_size_bytes_ == 1) {
        uint8_t offset = static_cast<uint8_t>(offset_);
        std::memcpy(buffer.data(), &offset, offset_channel_size_bytes_);
    } else if(offset_channel_size_bytes_ == 2) {
        uint16_t offset = static_cast<uint16_t>(offset_);
        std::memcpy(buffer.data(), &offset, offset_channel_size_bytes_);
    } else if(offset_channel_size_bytes_ == 4) {
        uint32_t offset = static_cast<uint32_t>(offset_);
        std::memcpy(buffer.data(), &offset, offset_channel_size_bytes_);
    } else if(offset_channel_size_bytes_ == 8) {
        uint64_t offset = static_cast<uint64_t>(offset_);
        std::memcpy(buffer.data(), &offset, offset_channel_size_bytes_);
    } else {
        throw std::runtime_error("Invalid offset size bytes");
    }

    return buffer;
}

uint64_t VlsdDataRecord::WriteToStream(std::streambuf& stream, const std::span<const uint8_t>& data) {
    const auto record_data = Create(data);
    uint64_t record_size_bytes = GetRecordSizeBytes(data.size());

    auto ret = stream.sputn(
        reinterpret_cast<const char*>(record_data.get()),
        static_cast<std::streamsize>(record_size_bytes));

    if(ret != record_size_bytes)
        throw std::ios_base::failure("End of stream reached (EOF).");

    offset_ += 4 + data.size();

    return ret;
}

} // namespace eerie_leap::subsys::mdf::mdf4
