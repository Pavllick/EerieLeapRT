#pragma once

#include <chrono>

namespace eerie_leap::subsys::time {

using namespace std::chrono;

class ITimeProvider {
public:
    virtual ~ITimeProvider() = default;

    system_clock::time_point virtual GetTime() = 0;
};

} // namespace eerie_leap::subsys::time
