#pragma once

#include <chrono>

namespace eerie_leap::utilities::time {

using namespace std::chrono;

class ITimeService {
protected:
    bool initialized = false;

public:
    void virtual Initialize() = 0;
    system_clock::time_point virtual GetCurrentTime() = 0;
};

} // namespace eerie_leap::utilities::time