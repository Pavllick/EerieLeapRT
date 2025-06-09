#pragma once

#include <zephyr/net/http/service.h>

namespace eerie_leap::domain::http_domain::controllers::view {

class SensorsEditorController {
public:
    static http_resource_detail_static sensors_config_editor_html_resource_detail;

    SensorsEditorController() = delete;
    ~SensorsEditorController() = delete;
};

} // namespace eerie_leap::domain::http_domain::controllers::view
