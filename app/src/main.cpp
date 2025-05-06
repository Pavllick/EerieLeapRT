#include <memory>
#include <chrono>
#include <ctime>
#include <stdio.h>
#include <zephyr/arch/cpu.h>
#include <zephyr/logging/log.h>

#include "utilities/dev_tools/system_info.h"
#include "utilities/time/i_time_service.h"
#include "utilities/guid/guid_generator.h"
#include "utilities/time/time_helpers.hpp"
#include "utilities/time/boot_elapsed_time_service.h"
#include "controllers/sensors_configuration_controller.h"
#include "domain/fs_domain/services/i_fs_service.h"
#include "domain/fs_domain/services/fs_service.h"
#include "domain/sensor_domain/services/measurement_service.h"

#include "configuration/system_config/system_config.h"
#include "configuration/sensor_config/sensor_config.h"
#include "configuration/services/configuration_service.hpp"

using namespace std::chrono;
using namespace eerie_leap::utilities::dev_tools;
using namespace eerie_leap::utilities::time;
using namespace eerie_leap::utilities::guid;
using namespace eerie_leap::controllers;
using namespace eerie_leap::domain::sensor_domain::services;

using namespace eerie_leap::domain::fs_domain::services;
using namespace eerie_leap::utilities::cbor;
using namespace eerie_leap::configuration::services;

#define SLEEP_TIME_MS 10000

int main(void) {
    std::shared_ptr<IFsService> fs_service = std::make_shared<FsService>();

    auto time_service = std::make_shared<BootElapsedTimeService>();
    time_service->Initialize();

    auto guid_generator = std::make_shared<GuidGenerator>();
    
    auto system_config_service = std::make_shared<ConfigurationService<SystemConfig>>("system_config", fs_service);
    auto sensors_config_service = std::make_shared<ConfigurationService<SensorsConfig>>("sensors_config", fs_service);

    auto sensors_configuration_controller = std::make_shared<SensorsConfigurationController>(sensors_config_service);

    // SystemConfig system_config;
    // system_config.hw_version = 22;
    // system_config.sw_version = 2422;

    // auto save_res = system_configuration_service->Save(system_config);
    // if (save_res) {
    //     printf("Configuration saved successfully\n");
    // } else {
    //     printf("Failed to save configuration\n");
    // }

    // auto loaded_config = system_configuration_service->Load();
    // if (loaded_config.has_value()) {
    //     printf("Loaded hw_version: %d, sw_version: %d\n", loaded_config.value().hw_version, loaded_config.value().sw_version);
    // } else {
    //     printf("Failed to load configuration\n");
    // }

    // Placement-new construction of MeasurementService in statically allocated,
    // properly aligned memory. This ensures the internal Zephyr thread stack
    // (defined via K_KERNEL_STACK_MEMBER) is in valid memory and survives for
    // the lifetime of the thread.
    //
    // DO NOT allocate this on the heap or stack — it will crash due to stack
    // alignment or lifetime issues in Zephyr.
    alignas(ARCH_STACK_PTR_ALIGN) static uint8_t service_buffer[sizeof(MeasurementService)];
    auto* service = new (service_buffer) MeasurementService(time_service, guid_generator, sensors_configuration_controller);
    service->Start();

    // auto files = fs_service->ListFiles("/");
    // for(const auto& file : files)
    //     printf("File name: %s\n", file.c_str());

    // printf("Total memory: %d, Used memory: %d\n", fs_service->GetTotalSpace(), fs_service->GetUsedSpace());

    while (true) {
        SystemInfo::print_heap_info();
        SystemInfo::print_stack_info();
        k_msleep(SLEEP_TIME_MS);
    }

    return 0;
}