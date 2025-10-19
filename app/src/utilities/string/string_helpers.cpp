#include "string_helpers.h"

namespace eerie_leap::utilities::string {

std::unique_ptr<char[]> StringHelpers::ToPaddedCharArray(const std::string& str, size_t size, char padding_char) {
    std::unique_ptr<char[]> char_array = std::make_unique<char[]>(size);
    memset(char_array.get(), padding_char, size);
    std::copy(str.begin(), str.end(), char_array.get());

    return char_array;
}

} // namespace eerie_leap::utilities::string
