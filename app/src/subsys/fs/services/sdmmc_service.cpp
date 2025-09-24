#include <filesystem>
#include <ff.h>

#include <zephyr/fs/fs.h>
#include <zephyr/logging/log.h>
#include <zephyr/storage/disk_access.h>
#ifdef CONFIG_LOG_BACKEND_FS
#include <zephyr/logging/log_backend.h>
#include <zephyr/logging/log_backend_fs.h>
#endif // CONFIG_LOG_BACKEND_FS

#include "subsys/device_tree/dt_fs.h"

#include "sdmmc_service.h"

namespace eerie_leap::subsys::fs::services {

using namespace eerie_leap::subsys::device_tree;

LOG_MODULE_REGISTER(sdmmc_service_logger);

// #ifdef CONFIG_SHARED_MULTI_HEAP
// Z_KERNEL_STACK_DEFINE_IN(SdmmcService::stack_area_, SdmmcService::k_stack_size_, __attribute__((section(".ext_ram.bss"))));
// #else
// K_KERNEL_STACK_MEMBER(SdmmcService::stack_area_, SdmmcService::k_stack_size_);
// #endif

SdmmcService::SdmmcService(fs_mount_t mountpoint) : FsService(mountpoint), monitor_running_(ATOMIC_INIT(0)) { }

SdmmcService::~SdmmcService() {
    k_thread_join(thread_id_, K_FOREVER);
    k_thread_stack_free(stack_area_);
}

bool SdmmcService::Initialize() {
    sd_card_present_ = FsService::Initialize();
    if(!sd_card_present_)
        return false;

    PrintInfo();
    return true;
}

bool SdmmcService::IsAvailable() const {
    return sd_card_present_;
}

bool SdmmcService::IsSdCardAttached(const char* disk_name) {
    bool bool_true = true;
    disk_access_ioctl(disk_name, DISK_IOCTL_CTRL_DEINIT, &bool_true);

    return disk_access_ioctl(disk_name, DISK_IOCTL_CTRL_INIT, nullptr) == 0;
}

void SdmmcService::SdMonitorHandler() {
    bool card_detected = DtFs::IsSdCardPresent();

    if(!card_detected) {
        const char* disk_name = DtFs::GetSdDiskName();
        IsSdCardAttached(disk_name);
    }

    if(card_detected != sd_card_present_) {
        sd_card_present_ = card_detected;

        if(card_detected) {
            LOG_INF("SD card detected.");

            Unmount();
            if(!Mount()) {
                sd_card_present_ = false;
            }
        } else {
            LOG_INF("SD card removed.");
        }
    }

#ifdef CONFIG_LOG_BACKEND_FS
    if(sd_card_present_ && log_backend_fs_get_state() == BACKEND_FS_CORRUPTED)
        log_backend_init(log_backend_fs_get());
#endif // CONFIG_LOG_BACKEND_FS
}

void SdmmcService::SdMonitorThreadEntry() {
    while(monitor_running_) {
        SdMonitorHandler();

        if(k_sem_take(&sd_monitor_stop_sem_, K_MSEC(CONFIG_EERIE_LEAP_SD_CHECK_INTERVAL_MS)) == 0)
            break;
    }
}

int SdmmcService::SdMonitorStart() {
    k_sem_init(&sd_monitor_stop_sem_, 0, 1);
    atomic_set(&monitor_running_, 1);

    stack_area_ = k_thread_stack_alloc(k_stack_size_, 0);
    thread_id_ = k_thread_create(
        &thread_data_,
        stack_area_,
        k_stack_size_,
        [](void* instance, void* p2, void* p3) { static_cast<SdmmcService*>(instance)->SdMonitorThreadEntry(); },
        this, nullptr, nullptr,
        k_priority_, 0, K_NO_WAIT);

    k_thread_name_set(&thread_data_, "sd_monitor");

    LOG_INF("SD card monitoring started.");

    return 0;
}

int SdmmcService::SdMonitorStop() {
    if(!atomic_get(&monitor_running_))
        return 0;

    atomic_set(&monitor_running_, 0);
    k_sem_give(&sd_monitor_stop_sem_);
    k_thread_join(&thread_data_, K_FOREVER);

    LOG_INF("SD card monitoring stopped.");

    return 0;
}

int SdmmcService::PrintInfo() const {
    const char* disk_name = DtFs::GetSdDiskName();
    uint64_t memory_size_mb = 0;
    uint32_t block_count = 0;
    uint32_t block_size = 0;

    int ret = disk_access_ioctl(disk_name, DISK_IOCTL_GET_SECTOR_COUNT, &block_count);
    if(ret != 0) {
        LOG_ERR("Unable to get sector count (%d)", ret);
        return ret;
    }

    ret = disk_access_ioctl(disk_name, DISK_IOCTL_GET_SECTOR_SIZE, &block_size);
    if(ret != 0) {
        LOG_ERR("Unable to get sector size (%d)", ret);
        return ret;
    }

    memory_size_mb = (uint64_t)block_count * block_size / (1024 * 1024);
    LOG_INF("SD card: %llu MB, %u sectors, %u bytes/sector",
        memory_size_mb, block_count, block_size);

    return 0;
}

} // namespace eerie_leap::subsys::fs::services
