#include <ctime>
#include <zephyr/kernel.h>

#include "boot_elapsed_time_provider.h"

namespace eerie_leap::subsys::time {

system_clock::time_point BootElapsedTimeProvider::GetTime() {
	return system_clock::time_point(milliseconds(k_uptime_get()));
}

} // namespace eerie_leap::subsys::time
