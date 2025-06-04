#pragma once

#include <zephyr/kernel.h>

static constexpr uint16_t http_service_port_ = 8080;

namespace eerie_leap::domain::http_domain::services {

class HttpServer {
public:
    HttpServer() = delete;
    ~HttpServer() = delete;

    static void Start();
};

} // namespace eerie_leap::domain::http_domain::services
