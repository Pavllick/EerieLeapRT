#include <cstring>
#include <utility>

#include "utilities/string/string_helpers.h"

#include "id_block.h"

namespace eerie_leap::subsys::mdf::mdf4 {

using namespace eerie_leap::utilities::string;

IdBlock::IdBlock(bool is_finalized): is_finalized_(is_finalized) {
    id_ = is_finalized_ ? "MDF" : "UnFinMF";
    version_str_ = "4.10";
    program_id_ = "EL";
    byte_order_ = 0;
    floating_point_format_ = 0;
    version_num_ = 410;
    code_page_number_ = 0;
    standard_flags_ = 0;
    custom_flags_ = 0;
}

uint64_t IdBlock::GetBlockSize() const {
    return 8 + 8 + 8 + 2 + 2 + 2 + 2 + 2 + 26 + 2 + 2; // = 64 bytes
}

std::unique_ptr<uint8_t[]> IdBlock::Serialize() const {
    const uint64_t size = GetBlockSize();
    auto buffer = std::make_unique<uint8_t[]>(size);
    std::memset(buffer.get(), 0, size);

    uint64_t offset = 0;

    auto id_char_array = StringHelpers::ToPaddedCharArray(id_, 8);
    std::copy(id_char_array.get(), id_char_array.get() + 8, buffer.get() + offset);
    offset += 8;

    auto version_str_char_array = StringHelpers::ToPaddedCharArray(version_str_, 8);
    std::copy(version_str_char_array.get(), version_str_char_array.get() + 8, buffer.get() + offset);
    offset += 8;

    auto program_id_char_array = StringHelpers::ToPaddedCharArray(program_id_, 8);
    std::copy(program_id_char_array.get(), program_id_char_array.get() + 8, buffer.get() + offset);
    offset += 8;

    std::memcpy(buffer.get() + offset, &byte_order_, sizeof(byte_order_));
    offset += sizeof(byte_order_);

    std::memcpy(buffer.get() + offset, &floating_point_format_, sizeof(floating_point_format_));
    offset += sizeof(floating_point_format_);

    std::memcpy(buffer.get() + offset, &version_num_, sizeof(version_num_));
    offset += sizeof(version_num_);

    std::memcpy(buffer.get() + offset, &code_page_number_, sizeof(code_page_number_));
    offset += sizeof(code_page_number_);

    offset += 2; // reserved_0_
    offset += 26; // reserved_1_

    std::memcpy(buffer.get() + offset, &standard_flags_, sizeof(standard_flags_));
    offset += sizeof(standard_flags_);

    std::memcpy(buffer.get() + offset, &custom_flags_, sizeof(custom_flags_));
    offset += sizeof(custom_flags_);

    return buffer;
}

void IdBlock::AddStandardFlag(StandardFlag flag) {
    standard_flags_ |= std::to_underlying(flag);
}

void IdBlock::ClearStandardFlags() {
    standard_flags_ = 0;
}

} // namespace eerie_leap::subsys::mdf::mdf4
