#pragma once

namespace eerie_leap::domain::logging_domain::models {

struct SensorLoggingConfiguration {
    bool is_enabled = false;
    bool log_raw_value = false;
    bool log_only_new_data = false;
};

} // namespace eerie_leap::domain::logging_domain::models
