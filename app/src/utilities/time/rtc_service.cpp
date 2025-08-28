#include <ctime>
#include <zephyr/kernel.h>

#include "rtc_service.h"

namespace eerie_leap::utilities::time {

system_clock::time_point RtcService::GetTime()
{
	return system_clock::time_point(milliseconds(k_uptime_get()));
}

} // namespace eerie_leap::utilities::time
