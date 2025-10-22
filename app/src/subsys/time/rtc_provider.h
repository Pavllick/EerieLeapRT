#pragma once

#include <chrono>

#include "i_time_provider.h"

namespace eerie_leap::subsys::time {

class RtcProvider : public ITimeProvider {
public:
    system_clock::time_point GetTime() override;
};

} // namespace eerie_leap::subsys::time
