#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include "subsys/mdf/serializable_block_base.h"

namespace eerie_leap::subsys::mdf::mdf4 {

class IdBlock : public SerializableBlockBase {
public:
    enum class StandardFlag : uint16_t {
        None = 0x00,
        InvalidCGCount = 0x01,          // Update CG(Channel group) count
        InvalidSRCount = 0x02,          // Update SR(Sample reduction) Count
        InvalidLastDTBlock = 0x04,      // Update length of last DT(Data) Block
        InvalidLastRDBlock = 0x08,      // Update length of last RD(Reduction data) Block
        InvalidLastDLBlock = 0x10,      // Update length of last DL(Data list) Block
        InvalidDataVLSDBlock = 0x20,    // Update length of data VLSD(Variable length signal data channel) Block
        InvalidOffsetVLSDBlock = 0x40   // Update length of offset VLSD(Variable length signal data channel) Block
    };

private:
    std::string id_;                        // 8 bytes, File identifier
    std::string version_str_;               // 8 bytes, Format version
    std::string program_id_;                // 8 bytes, Program identifier
    uint16_t byte_order_;                   // 2 bytes, Byte order, 0 = little-endian, 1 = big-endian
    uint16_t floating_point_format_;        // 2 bytes, Floating point format, 0 = IEEE 754
    uint16_t version_num_;                  // 2 bytes, Version number
    uint16_t code_page_number_;             // 2 bytes, Code page number
    // uint8_t reserved_0_[2];              // 2 bytes, Reserved
    // uint8_t reserved_1_[26];             // 26 bytes, Reserved
    uint16_t standard_flags_;               // 2 bytes, Flags
    uint16_t custom_flags_;                 // 2 bytes, Flags

    bool is_finalized_;

public:
    IdBlock(bool is_finalized = true);
    virtual ~IdBlock() = default;

    uint64_t GetBlockSize() const override;
    std::unique_ptr<uint8_t[]> Serialize() const override;
    std::vector<std::shared_ptr<ISerializableBlock>> GetChildren() const override { return {}; }

    void AddStandardFlag(StandardFlag flag);
    void ClearStandardFlags();
};

} // namespace eerie_leap::subsys::mdf::mdf4
