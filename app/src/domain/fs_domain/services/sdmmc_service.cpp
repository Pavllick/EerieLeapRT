#include <sstream>
#include <filesystem>

#include <zephyr/fs/fs.h>
#include <zephyr/logging/log.h>
#include <zephyr/storage/disk_access.h>

#include "utilities/constants.h"
#include "sdmmc_service.h"

namespace eerie_leap::domain::fs_domain::services {

using namespace eerie_leap::utilities::constants;

LOG_MODULE_REGISTER(sdmmc_service_logger);

FS_LITTLEFS_DECLARE_DEFAULT_CONFIG(sd_storage_);

SdmmcService::SdmmcService(const std::string& disk_name, const std::string& mount_point) : disk_name_(disk_name) {
    memset(&mountpoint_, 0, sizeof(mountpoint_));

    std::filesystem::path full_path(mount_point);
    mount_point_ = full_path.string();

    mountpoint_.type = FS_LITTLEFS;
    mountpoint_.mnt_point = mount_point_.c_str();
    mountpoint_.fs_data = &sd_storage_;
    mountpoint_.storage_dev = (void *)disk_name_.c_str();
    mountpoint_.flags = FS_MOUNT_FLAG_USE_DISK_ACCESS;

    sd_monitor_task_ = CreateSdMonitorTask();
}

bool SdmmcService::Initialize() {
    sd_monitor_task_->sd_card_present_ = FsService::Initialize();
    SdMonitorStart();

    if(!sd_monitor_task_->sd_card_present_)
        return false;

    PrintInfo();

    return true;
}

std::shared_ptr<SdMonitorTask> SdmmcService::CreateSdMonitorTask() {
    auto task = std::make_shared<SdMonitorTask>();

    task->check_interval_ms = K_MSEC(fs::SD_CHECK_INTERVAL_MS);
    task->disk_name = disk_name_.c_str();
    task->sd_card_present_ = false;

    task->IsDiskAvailable = [this]() { return GetTotalSpace() > 0; };
    task->Mount = [this]() { return Mount(); };
    task->Unmount = [this]() { return Unmount(); };

    return task;
}

bool SdmmcService::IsSdCardAttached(const char* disk_name) {
    bool bool_true = true;
    disk_access_ioctl(disk_name, DISK_IOCTL_CTRL_DEINIT, &bool_true);

    return disk_access_ioctl(disk_name, DISK_IOCTL_CTRL_INIT, nullptr) == 0;
}

void SdmmcService::SdMonitorWorkHandler(struct k_work *work) {
    SdMonitorTask* task = CONTAINER_OF(work, SdMonitorTask, work);

    bool card_detected = task->IsDiskAvailable();

    if(!card_detected)
        card_detected = IsSdCardAttached(task->disk_name);

    if (card_detected != task->sd_card_present_) {
        task->sd_card_present_ = card_detected;

        if (card_detected) {
            LOG_INF("SD card detected.");

            if (task->Mount()) {
                LOG_INF("SD card mounted.");
            }
        } else {
            LOG_INF("SD card removed.");

            task->Unmount();
        }
    }

    k_work_reschedule(&task->work, task->check_interval_ms);
}

int SdmmcService::SdMonitorStart() {
    k_work_init_delayable(&sd_monitor_task_->work, SdMonitorWorkHandler);
    k_work_schedule(&sd_monitor_task_->work, sd_monitor_task_->check_interval_ms);

    LOG_INF("SD card monitoring started.");

    return 0;
}

int SdmmcService::SdMonitorStop() {
    k_work_cancel_delayable(&sd_monitor_task_->work);

    LOG_INF("SD card monitoring stopped.");

    return 0;
}

int SdmmcService::PrintInfo() const {
    const char *disk_pdrv = (const char*)mountpoint_.storage_dev;
    uint64_t memory_size_mb = 0;
    uint32_t block_count = 0;
    uint32_t block_size = 0;

    int ret = disk_access_ioctl(disk_pdrv, DISK_IOCTL_GET_SECTOR_COUNT, &block_count);
    if (ret != 0) {
        LOG_ERR("Unable to get sector count (%d)", ret);
        return ret;
    }

    ret = disk_access_ioctl(disk_pdrv, DISK_IOCTL_GET_SECTOR_SIZE, &block_size);
    if (ret != 0) {
        LOG_ERR("Unable to get sector size (%d)", ret);
        return ret;
    }

    memory_size_mb = (uint64_t)block_count * block_size / (1024 * 1024);
    LOG_INF("SD card: %llu MB, %u sectors, %u bytes/sector",
        memory_size_mb, block_count, block_size);
}

}
