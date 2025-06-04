#include <memory>
#include <ctime>
#include <zephyr/arch/cpu.h>
#include <zephyr/logging/log.h>

#include "utilities/dev_tools/system_info.h"
#include "utilities/guid/guid_generator.h"
#include "utilities/time/boot_elapsed_time_service.h"
#include "domain/fs_domain/services/fs_service.h"
#include "domain/sensor_domain/services/measurement_service.h"

#include "configuration/system_config/system_config.h"
#include "configuration/adc_config/adc_config.h"
#include "configuration/sensor_config/sensor_config.h"
#include "configuration/services/configuration_service.hpp"
#include "controllers/sensors_configuration_controller.h"
#include "controllers/adc_configuration_controller.h"
#include "controllers/system_configuration_controller.h"

#include "domain/http_domain/services/wifi_ap_service.h"
#include "domain/http_domain/services/http_server.h"

using namespace eerie_leap::utilities::dev_tools;
using namespace eerie_leap::utilities::time;
using namespace eerie_leap::utilities::guid;

using namespace eerie_leap::controllers;

using namespace eerie_leap::domain::sensor_domain::services;
using namespace eerie_leap::domain::fs_domain::services;
using namespace eerie_leap::configuration::services;
using namespace eerie_leap::domain::http_domain::services;

#define SLEEP_TIME_MS 10000

int main(void) {
    auto fs_service = std::make_shared<FsService>();

    auto time_service = std::make_shared<BootElapsedTimeService>();
    time_service->Initialize();

    auto guid_generator = std::make_shared<GuidGenerator>();

    auto system_config_service = std::make_shared<ConfigurationService<SystemConfig>>("system_config", fs_service);
    auto adc_config_service = std::make_shared<ConfigurationService<AdcConfig>>("adc_config", fs_service);
    auto sensors_config_service = std::make_shared<ConfigurationService<SensorsConfig>>("sensors_config", fs_service);

    auto system_configuration_controller = std::make_shared<SystemConfigurationController>(system_config_service);
    auto adc_configuration_controller = std::make_shared<AdcConfigurationController>(adc_config_service);
    auto sensors_configuration_controller = std::make_shared<SensorsConfigurationController>(sensors_config_service);

    // Placement-new construction of MeasurementService in statically allocated,
    // properly aligned memory. This ensures the internal Zephyr thread stack
    // (defined via K_KERNEL_STACK_MEMBER) is in valid memory and survives for
    // the lifetime of the thread.
    //
    // DO NOT allocate this on the heap or stack — it will crash due to stack
    // alignment or lifetime issues in Zephyr.
    // alignas(ARCH_STACK_PTR_ALIGN) static uint8_t measurement_service_buffer[sizeof(MeasurementService)];
    // auto* measurement_service = new (measurement_service_buffer) MeasurementService(time_service, guid_generator, sensors_configuration_controller);
    // measurement_service->Start();

    // NOTE: Don't use for WiFi supporting boards as WiFi is broken in Zephyr 4.1 and has memory allocation issues
    // At least on ESP32S3, it does connect if Zephyr revision is set to "main", but heap allocations cannot be moved
    // to the external RAM (e.g. PSRAM)
#ifdef CONFIG_WIFI
    WifiApService::Initialize();
#endif // CONFIG_WIFI

#ifdef CONFIG_NETWORKING
    HttpServer::Start();
#endif // CONFIG_NETWORKING

    // auto files = fs_service->ListFiles("/");
    // printf("Total memory: %d, Used memory: %d\n", fs_service->GetTotalSpace(), fs_service->GetUsedSpace());

    while (true) {
        SystemInfo::print_heap_info();
        SystemInfo::print_stack_info();
        k_msleep(SLEEP_TIME_MS);
    }

    return 0;
}
