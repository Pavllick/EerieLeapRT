#include <ctime>
#include <zephyr/kernel.h>

#include "rtc_service.h"

namespace eerie_leap::utilities::time {

system_clock::time_point RtcService::GetCurrentTime()
{
	uint64_t fake_start_time = 1761106217000;
	return system_clock::time_point(milliseconds(fake_start_time + k_uptime_get()));
}

system_clock::time_point RtcService::GetTimeSinceBoot()
{
	return GetCurrentTime();
}

} // namespace eerie_leap::utilities::time
