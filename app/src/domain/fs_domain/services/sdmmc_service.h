#pragma once

#include <string>
#include <vector>
#include <memory>
#include <functional>

#include <zephyr/device.h>
#include <zephyr/fs/fs.h>
#include <zephyr/fs/littlefs.h>

#include "fs_service.h"

namespace eerie_leap::domain::fs_domain::services {

struct SdMonitorTask {
    k_work_delayable work;
    k_timeout_t check_interval_ms;
    const char* disk_name;
    bool sd_card_present_;
    std::function<bool()> IsDiskAvailable;
    std::function<int()> Mount;
    std::function<void()> Unmount;
};

class SdmmcService : public FsService {
private:
    const std::string disk_name_;
    std::string mount_point_;
    std::shared_ptr<SdMonitorTask> sd_monitor_task_;

    std::shared_ptr<SdMonitorTask> CreateSdMonitorTask();
    static bool IsSdCardAttached(const char* disk_name);
    static void SdMonitorWorkHandler(struct k_work *work);
    int SdMonitorStart();
    int SdMonitorStop();
    int PrintInfo() const;

public:
    SdmmcService(const std::string& disk_name, const std::string& mount_point);

    bool Initialize() override;
};

}
