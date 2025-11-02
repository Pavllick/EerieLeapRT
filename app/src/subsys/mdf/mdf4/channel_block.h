#pragma once

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

#include "subsys/mdf/utilities/block_links.h"
#include "text_block.h"
#include "channel_conversion_block.h"
#include "block_base.h"

namespace eerie_leap::subsys::mdf::mdf4 {

using namespace eerie_leap::subsys::mdf::utilities;

class ChannelBlock : public BlockBase {
public:
    enum class Type: uint8_t {
        FixedLength = 0,     // Fixed length data (default type)
        VariableLength = 1,  // Variable length data
        Master = 2,          // Master channel
        VirtualMaster = 3,   // Virtual master channel
        Sync = 4,            // Synchronize channel
        MaxLength = 5,       // Max length channel
        VirtualData = 6      // Virtual data channel
    };

    enum class SyncType: uint8_t {
        NoSync = 0,     // No synchronization (default value)
        Time = 1,       // Time type
        Angle = 2,      // Angle type
        Distance = 3,   // Distance type
        Index = 4       // Sample number
    };

    // NOTE: Only currently supported types uncommented
    enum class DataType: uint8_t {
        UnsignedIntegerLe= 0,       // Unsigned integer, little endian.
        // UnsignedIntegerBe = 1,   // Unsigned integer, big endian.
        SignedIntegerLe = 2,        // Signed integer, little endian.
        // SignedIntegerBe = 3,     // Signed integer, big endian.
        FloatLe = 4,                // Float, little endian.
        // FloatBe = 5,             // Float, big endian.
        // StringAscii = 6,         // Text,  ISO-8859-1 coded
        // StringUTF8 = 7,          // Text, UTF8 coded.
        // StringUTF16Le = 8,       // Text, UTF16 coded little endian.
        // StringUTF16Be = 9,       // Text, UTF16 coded big endian.
        ByteArray = 10,             // Byte array.
        // MimeSample = 11,         // MIME sample byte array.
        // MimeStream = 12,         // MIME stream byte array.
        // CanOpenDate = 13,        // 7-byte CANOpen date.
        // CanOpenTime = 14,        // 6-byte CANOpen time.
        // ComplexLe = 15,          // Complex value, little endian.
        // ComplexBe = 16           // Complex value, big endian.
    };

    enum class Flag: uint32_t {
        Default = 0x0000,               // Default.
        AllValuesInvalid = 0x0001,      // All values are invalid.
        InvalidValid = 0x0002,          // Invalid bit is used.
        PrecisionValid = 0x0004,        // Precision is used.
        RangeValid = 0x0008,            // Range is used.
        LimitValid = 0x0010,            // Limit is used.
        ExtendedLimitValid = 0x0020,    // Extended limit is used.
        Discrete = 0x0040,              // Discrete channel.
        Calibration = 0x0080,           // Calibrated channel.
        Calculated = 0x0100,            // Calculated channel.
        Virtual = 0x0200,               // Virtual channel.
        BusEvent = 0x0400,              // Bus event channel.
        StrictlyMonotonous = 0x0800,    // Strict monotonously.
        DefaultX = 0x1000,              // Default x-axis channel.
        EventSignal = 0x2000,           // Event signal.
        VlsdDataStream = 0x4000         // VLSD data stream channel.
    };

private:
    enum class LinkType: int {
        ChannelNext = 0,
        ChannelArray,
        TextName,
        SourceInformation,
        ChannelConversion,
        SignalData,
        TextUnit,
        MetadataComment
    };

    BlockLinks<LinkType, 8> links_;

    Type type_;                         // 1 byte, Channel type
    SyncType sync_type_;                // 1 byte, Sync type
    DataType data_type_;                // 1 byte, Data type
    uint8_t bit_offset_;                // 1 byte, Bit offset
    uint32_t byte_offset_;              // 4 bytes, Byte offset
    uint32_t bit_count_;                // 4 bytes, Bit count
    uint32_t flags_;                    // 4 bytes, Flags
    uint32_t invalidation_bit_pos_;     // 4 bytes, Position of invalidation bit
    uint8_t precision_;                 // 1 byte, Precision
    // uint8_t reserved_1_[1];          // 1 bytes, Reserved
    uint16_t attachment_count_;         // 2 bytes, Number of attachments
    float val_range_min_;               // 8 bytes, Minimum value of raw range
    float val_range_max_;               // 8 bytes, Maximum value of raw range
    float limit_min_;                   // 8 bytes, Lower limit
    float limit_max_;                   // 8 bytes, Upper limit
    float limit_ext_min_;               // 8 bytes, Lower extended limit
    float limit_ext_max_;               // 8 bytes, Upper extended limit

public:
    ChannelBlock(Type type, SyncType sync_type, DataType data_type, uint32_t bit_count);
    virtual ~ChannelBlock() = default;

    uint32_t GetDataSizeBytes() const;
    uint32_t GetDataOffsetBytes() const;
    std::shared_ptr<ChannelBlock> GetLinkedChannel() const;
    void SetConversion(std::shared_ptr<ChannelConversionBlock> conversion);

    uint64_t GetBlockSize() const override;
    std::unique_ptr<uint8_t[]> Serialize() const override;
    const IBlockLinks* GetBlockLinks() const override { return &links_; }
    std::vector<std::shared_ptr<ISerializableBlock>> GetChildren() const override {
        return {
            links_.GetLink(LinkType::MetadataComment),
            links_.GetLink(LinkType::TextName),
            links_.GetLink(LinkType::TextUnit),
            links_.GetLink(LinkType::SourceInformation),
            links_.GetLink(LinkType::ChannelConversion),
            links_.GetLink(LinkType::ChannelArray),
            links_.GetLink(LinkType::SignalData),
            links_.GetLink(LinkType::ChannelNext) };
    }

    void LinkBlock(std::shared_ptr<ChannelBlock> next_block);
    void SetType(Type type);
    void SetFlags(uint32_t flags);
    void SetName(std::shared_ptr<TextBlock> name);
    void SetUnit(std::shared_ptr<TextBlock> unit);
    void SetArrayBlock(std::shared_ptr<IBlock> array_block);
    void SetOffsetBytes(uint32_t offset_bytes);
    void SetOffsetBits(uint8_t offset_bits);
    void SetBitCount(uint32_t bit_count);
    void SetSignalDataBlock(std::shared_ptr<IBlock> block);
};

} // namespace eerie_leap::subsys::mdf::mdf4
