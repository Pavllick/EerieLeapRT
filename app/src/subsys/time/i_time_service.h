#pragma once

#include <chrono>

namespace eerie_leap::subsys::time {

using namespace std::chrono;

class ITimeService {
public:
    virtual ~ITimeService() = default;

    system_clock::time_point virtual GetCurrentTime() = 0;
    system_clock::time_point virtual GetTimeSinceBoot() = 0;
};

} // namespace eerie_leap::subsys::time
