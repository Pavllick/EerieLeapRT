#pragma once

#include <string>
#include <chrono>

namespace eerie_leap::utilities::time {

using namespace std::chrono;

class TimeHelpers {
public:
    static std::string GetFormattedString(const std::chrono::system_clock::time_point& tp) {
        using namespace std::chrono;

        auto duration = tp.time_since_epoch();
        auto secs = duration_cast<seconds>(duration);
        auto millis = duration_cast<milliseconds>(duration - secs).count();

        std::time_t time_sec = system_clock::to_time_t(tp);
        std::tm* timeinfo = localtime(&time_sec);

        char buffer[32];
        std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", timeinfo);

        std::string formatted_time = std::string(buffer);
        return formatted_time + "." + std::to_string(millis);
    }
};

} // namespace eerie_leap::utilities::time
