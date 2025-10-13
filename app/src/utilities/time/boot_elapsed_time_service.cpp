#include <ctime>
#include <zephyr/kernel.h>

#include "boot_elapsed_time_service.h"

namespace eerie_leap::utilities::time {

system_clock::time_point BootElapsedTimeService::GetCurrentTime()
{
	return system_clock::time_point(milliseconds(k_uptime_get()));
}

system_clock::time_point BootElapsedTimeService::GetTimeSinceBoot()
{
	return GetCurrentTime();
}

} // namespace eerie_leap::utilities::time
