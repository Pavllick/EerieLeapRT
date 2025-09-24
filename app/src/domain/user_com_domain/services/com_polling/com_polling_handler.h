#pragma once

#include <functional>

namespace eerie_leap::domain::user_com_domain::services::com_polling {

using ComPollingHandler = std::function<int()>;

} // namespace eerie_leap::domain::user_com_domain::services::com_polling
