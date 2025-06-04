#include <zephyr/logging/log.h>
#include <zephyr/net/http/server.h>
#include <zephyr/net/http/service.h>

#include "http_server.h"

// Include the static web page (generated with `xxd -i`)
static const uint8_t index_html[] = {
    #include "index.html.gz.inc"
};

HTTP_SERVICE_DEFINE(http_service, "0.0.0.0", &http_service_port_, 1, 10, nullptr, nullptr);

namespace eerie_leap::domain::http_domain::services {

LOG_MODULE_REGISTER(http_server);

void HttpServer::Start() {
    int ret = http_server_start();
    if (ret != 0)
        LOG_ERR("Failed to start HTTP server, error: %d", ret);
    else
        LOG_INF("HTTP server started on port %u", http_service_port_);
}

static struct http_resource_detail_static index_html_resource_detail = {
    .common = {
        .bitmask_of_supported_http_methods = BIT(HTTP_GET),
        .type = HTTP_RESOURCE_TYPE_STATIC,
        .content_encoding = "gzip",
        .content_type = "text/html; charset=utf-8",
    },
    .static_data = index_html,
    .static_data_len = sizeof(index_html),
};

HTTP_RESOURCE_DEFINE(index_html_resource, http_service, "/", &index_html_resource_detail);

} // namespace eerie_leap::domain::http_domain::services
