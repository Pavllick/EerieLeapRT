#include "utilities/string/string_helpers.h"

#include "text_block_base.h"

namespace eerie_leap::subsys::mdf::mdf4 {

using namespace eerie_leap::utilities::string;

TextBlockBase::TextBlockBase(const std::string& id): BlockBase(id) {}

size_t TextBlockBase::GetAllignedTextSize() const {
    return text_.size() + (8 - text_.size() % 8);
}

uint64_t TextBlockBase::GetBlockSize() const {
    return GetBaseSize() + GetAllignedTextSize();
}

std::unique_ptr<uint8_t[]> TextBlockBase::Serialize() const {
    auto buffer = SerializeBase();
    uint64_t offset = GetBaseSize();

    auto text_size = GetAllignedTextSize();
    auto text_char_array = StringHelpers::ToPaddedCharArray(text_, text_size, '\0');
    std::copy(text_char_array.get(), text_char_array.get() + text_size, buffer.get() + offset);
    offset += text_size;

    return buffer;
}


} // namespace eerie_leap::subsys::mdf::mdf4
