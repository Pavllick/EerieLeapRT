#include <zephyr/logging/log.h>

#include "time_service.h"

namespace eerie_leap::subsys::time {

LOG_MODULE_REGISTER(time_service_logger);

TimeService::TimeService(std::shared_ptr<ITimeProvider> rtc_provider, std::shared_ptr<ITimeProvider> boot_elapsed_time_provider)
    : rtc_provider_(std::move(rtc_provider)), boot_elapsed_time_provider_(std::move(boot_elapsed_time_provider)) { }

void TimeService::Initialize() {
    LOG_INF("Time Service initialized");
}

system_clock::time_point TimeService::GetCurrentTime() {
    return rtc_provider_->GetTime();
}

system_clock::time_point TimeService::GetTimeSinceBoot() {
    return boot_elapsed_time_provider_->GetTime();
}

} // namespace eerie_leap::subsys::time
