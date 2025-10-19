#pragma once

#include <cstddef>
#include <cstring>
#include <memory>
#include <stddef.h>
#include <string>

namespace eerie_leap::utilities::string {

class StringHelpers {
public:
    static std::unique_ptr<char[]> ToPaddedCharArray(const std::string& str, size_t size, char padding_char = ' ');
};

} // namespace eerie_leap::utilities::string
