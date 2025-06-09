#include "sensors_editor_controller.h"

static const uint8_t sensors_config_editor_html[] = {
    #include "sensors_config_editor/bundle.html.gz.inc"
};

namespace eerie_leap::domain::http_domain::controllers::view {

http_resource_detail_static SensorsEditorController::sensors_config_editor_html_resource_detail = {
    .common = {
        .bitmask_of_supported_http_methods = BIT(HTTP_GET),
        .type = HTTP_RESOURCE_TYPE_STATIC,
        .content_encoding = "gzip",
        .content_type = "text/html; charset=utf-8",
    },
    .static_data = sensors_config_editor_html,
    .static_data_len = sizeof(sensors_config_editor_html),
};

} // namespace eerie_leap::domain::http_domain::controllers::view
