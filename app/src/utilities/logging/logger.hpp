#pragma once

#include <string_view>
#include <string>
#include <stdio.h>
#include <zephyr/logging/log.h>

// This LOG_MODULE_DECLARE should ideally be inside the namespace if all its users are,
// or outside if it's truly global. Given LOG_MODULE_REGISTER is global,
// LOG_MODULE_DECLARE is also typically used in a way that links to that global registration.
// For now, keeping it outside the C++ namespace as it interacts with C macros.
LOG_MODULE_DECLARE(logger, CONFIG_APP_LOGGER_LOG_LEVEL);

namespace eerie_leap::utilities::logging { // Added ::logging namespace

class Logger { // Renamed from AppLogger
public:
    explicit Logger(std::string_view name) : name_(name) {} // Constructor name updated

    template <typename... Args>
    void Debug(const char* format, Args... args) const {
        log(LOG_LEVEL_DBG, format, args...);
    }

    template <typename... Args>
    void Info(const char* format, Args... args) const {
        log(LOG_LEVEL_INF, format, args...);
    }

    template <typename... Args>
    void Warning(const char* format, Args... args) const {
        log(LOG_LEVEL_WRN, format, args...);
    }

    template <typename... Args>
    void Error(const char* format, Args... args) const {
        log(LOG_LEVEL_ERR, format, args...);
    }

private:
    template <typename... Args>
    void log(uint8_t level, const char* format, Args... args) const {
        if (level > LOG_LEVEL_GET(logger)) { // 'logger' here refers to the LOG_MODULE_DECLARE'd module
            return;
        }

        char prefixed_format[256];
        snprintf(prefixed_format, sizeof(prefixed_format), "[%s] %s", std::string(name_).c_str(), format);

        switch (level) {
            case LOG_LEVEL_ERR:
                LOG_ERR(prefixed_format, args...);
                break;
            case LOG_LEVEL_WRN:
                LOG_WRN(prefixed_format, args...);
                break;
            case LOG_LEVEL_INF:
                LOG_INF(prefixed_format, args...);
                break;
            case LOG_LEVEL_DBG:
                LOG_DBG(prefixed_format, args...);
                break;
        }
    }

    std::string_view name_;
};

} // namespace eerie_leap::utilities::logging
