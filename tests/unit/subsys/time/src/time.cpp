#include <memory>
#include <regex>
#include <zephyr/kernel.h>
#include <zephyr/ztest.h>

#include "subsys/time/time_helpers.hpp"
#include "subsys/time/i_time_provider.h"
#include "subsys/time/boot_elapsed_time_provider.h"

using namespace eerie_leap::subsys::time;

ZTEST_SUITE(time, NULL, NULL, NULL, NULL, NULL);

ZTEST(time, test_GetFormattedString_not_empty) {
    std::shared_ptr<ITimeProvider> time_provider = std::make_shared<BootElapsedTimeProvider>();
    auto current_time = time_provider->GetTime();

    auto formatted_time = TimeHelpers::GetFormattedString(current_time);

    zassert_not_equal(formatted_time.size(), 0);
    const std::regex time_pattern(R"(\d{4}-\d{2}-\d{2} \d{2}:\d{2}:\d{2}.*)");
    zassert_true(std::regex_match(formatted_time, time_pattern));
}

ZTEST(time, test_GetCurrentTime_returns_valid_elapsed_time) {
    std::shared_ptr<ITimeProvider> time_provider = std::make_shared<BootElapsedTimeProvider>();

    auto current_time1 = time_provider->GetTime();
    k_msleep(1);
    auto current_time2 = time_provider->GetTime();

    zassert_true(current_time2 - current_time1 >= 1ms);
    zassert_true(current_time2 - current_time1 <= 2ms);
}
