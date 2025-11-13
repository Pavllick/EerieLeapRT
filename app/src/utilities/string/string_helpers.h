#pragma once

#include <cstddef>
#include <cstring>
#include <memory>
#include <stddef.h>
#include <string>

namespace eerie_leap::utilities::string {

class StringHelpers {
private:
    static std::hash<std::string> string_hasher;

public:
    static std::unique_ptr<char[]> ToPaddedCharArray(const std::string& str, size_t size, char padding_char = ' ');
    static size_t GetHash(const std::string& str);
};

} // namespace eerie_leap::utilities::string
