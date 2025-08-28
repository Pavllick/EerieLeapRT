#pragma once

#include <memory>
#include <chrono>

#include "i_time_service.h"
#include "rtc_service.h"
#include "boot_elapsed_time_service.h"

namespace eerie_leap::utilities::time {

using namespace std::chrono;

class TimeService : public ITimeService {
private:
    std::shared_ptr<RtcService> rtc_service_;
    std::shared_ptr<BootElapsedTimeService> boot_elapsed_time_service_;

public:
    TimeService(std::shared_ptr<RtcService> rtc_service, std::shared_ptr<BootElapsedTimeService> boot_elapsed_time_service);

    void Initialize();

    system_clock::time_point GetCurrentTime() override;
    system_clock::time_point GetTimeSinceBoot() override;
};

} // namespace eerie_leap::utilities::time
