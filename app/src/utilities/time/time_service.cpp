#include <zephyr/logging/log.h>

#include "time_service.h"

namespace eerie_leap::utilities::time {

LOG_MODULE_REGISTER(time_service_logger);

TimeService::TimeService(std::shared_ptr<RtcService> rtc_service, std::shared_ptr<BootElapsedTimeService> boot_elapsed_time_service)
    : rtc_service_(std::move(rtc_service)), boot_elapsed_time_service_(std::move(boot_elapsed_time_service)) { }

void TimeService::Initialize() {
    LOG_INF("Time Service initialized");
}

system_clock::time_point TimeService::GetCurrentTime() {
    return rtc_service_->GetTime();
}

system_clock::time_point TimeService::GetTimeSinceBoot() {
    return boot_elapsed_time_service_->GetTime();
}

} // namespace eerie_leap::utilities::time
