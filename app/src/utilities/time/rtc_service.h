#pragma once

#include <chrono>

namespace eerie_leap::utilities::time {

using namespace std::chrono;

class RtcService {
public:
    system_clock::time_point GetTime();
};

} // namespace eerie_leap::utilities::time
