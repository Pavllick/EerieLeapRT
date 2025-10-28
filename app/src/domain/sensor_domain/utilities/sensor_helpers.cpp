#include <algorithm>

#include "sensor_helpers.h"

namespace eerie_leap::domain::sensor_domain::utilities {

bool SensorHelpers::IsIdValid(const std::string& id) {
    if(id.empty())
        return false;

    std::string valid_symbols = "_";

    if(!std::isalpha(id[0]) && valid_symbols.find(id[0]) == std::string::npos)
        return false;

    return std::ranges::all_of(id, [&valid_symbols](char c) {
        return std::isalnum(c) || valid_symbols.find(c) != std::string::npos;
    });
}

} // namespace eerie_leap::domain::sensor_domain::utilities
