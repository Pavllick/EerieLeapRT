#pragma once

#include <chrono>

#include "i_time_service.h"

namespace eerie_leap::utilities::time {

using namespace std::chrono;

class RtcService : public ITimeService {
public:
    system_clock::time_point GetCurrentTime() override;
    system_clock::time_point GetTimeSinceBoot() override;
};

} // namespace eerie_leap::utilities::time
