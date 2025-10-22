#include <ctime>
#include <zephyr/kernel.h>

#include "rtc_provider.h"

namespace eerie_leap::subsys::time {

system_clock::time_point RtcProvider::GetTime() {
	uint64_t fake_start_time = 1761106217000;
	return system_clock::time_point(milliseconds(fake_start_time + k_uptime_get()));
}

} // namespace eerie_leap::subsys::time
