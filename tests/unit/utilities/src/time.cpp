#include <memory>
#include <regex>
#include <zephyr/kernel.h>
#include <zephyr/ztest.h>

#include "utilities/time/time_helpers.hpp"
#include "utilities/time/i_time_service.h"
#include "utilities/time/boot_elapsed_time_service.h"

using namespace eerie_leap::utilities::time;

ZTEST_SUITE(time, NULL, NULL, NULL, NULL, NULL);

ZTEST(time, test_GetFormattedString_not_empty) {
    std::shared_ptr<ITimeService> time_service = std::make_shared<BootElapsedTimeService>();
    auto current_time = time_service->GetCurrentTime();

    auto formatted_time = TimeHelpers::GetFormattedString(current_time);

    zassert_not_equal(formatted_time.size(), 0);
    const std::regex time_pattern(R"(\d{4}-\d{2}-\d{2} \d{2}:\d{2}:\d{2}.*)");
    zassert_true(std::regex_match(formatted_time, time_pattern));
}

ZTEST(time, test_GetCurrentTime_returns_valid_elapsed_time) {
    std::shared_ptr<ITimeService> time_service = std::make_shared<BootElapsedTimeService>();

    auto current_time1 = time_service->GetCurrentTime();
    k_msleep(1);
    auto current_time2 = time_service->GetCurrentTime();

    zassert_true(current_time2 - current_time1 >= 1ms);
    zassert_true(current_time2 - current_time1 <= 2ms);
}
