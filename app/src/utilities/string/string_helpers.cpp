#include "string_helpers.h"

namespace eerie_leap::utilities::string {

std::hash<std::string_view> StringHelpers::string_hasher;

std::unique_ptr<char[]> StringHelpers::ToPaddedCharArray(const std::string& str, size_t size, char padding_char) {
    std::unique_ptr<char[]> char_array = std::make_unique<char[]>(size);
    memset(char_array.get(), padding_char, size);
    std::copy(str.begin(), str.end(), char_array.get());

    return char_array;
}

size_t StringHelpers::GetHash(std::string_view str) {
    return string_hasher(str);
}

} // namespace eerie_leap::utilities::string
