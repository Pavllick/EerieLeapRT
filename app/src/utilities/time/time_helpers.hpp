#pragma once

#include <chrono>

namespace eerie_leap::utilities::time {

using namespace std::chrono;

class TimeHelpers {
public:
    static void PrintTimePoint(const std::chrono::system_clock::time_point& tp) {
        using namespace std::chrono;
    
        auto duration = tp.time_since_epoch();
        auto secs = duration_cast<seconds>(duration);
        auto millis = duration_cast<milliseconds>(duration - secs).count();
    
        std::time_t time_sec = system_clock::to_time_t(tp);
        std::tm timeinfo;
        localtime_r(&time_sec, &timeinfo);
    
        char buffer[32];
        std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", &timeinfo);
    
        std::printf("%s.%03lld\n", buffer, static_cast<long long>(millis));
    }
};

} // namespace eerie_leap::utilities::time