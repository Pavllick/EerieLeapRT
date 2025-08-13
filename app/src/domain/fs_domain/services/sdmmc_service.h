#pragma once

#include <string>
#include <memory>
#include <atomic>

#include <zephyr/device.h>
#include <zephyr/fs/fs.h>
#include <zephyr/fs/littlefs.h>

#include "fs_service.h"

namespace eerie_leap::domain::fs_domain::services {

class SdmmcService : public FsService {
private:
    const std::string disk_name_;
    std::string mount_point_;

    bool sd_card_present_ = false;
    k_sem sd_monitor_stop_sem_;
    std::atomic<bool> monitor_running_ = false;

    static constexpr int k_stack_size_ = CONFIG_EERIE_LEAP_FS_SD_THREAD_STACK_SIZE;
    static constexpr int k_priority_ = K_PRIO_COOP(7);

    static z_thread_stack_element stack_area_[k_stack_size_];
    k_tid_t thread_id_;
    k_thread thread_data_;

    void SdMonitorThreadEntry();

    static bool IsSdCardAttached(const char* disk_name);
    void SdMonitorHandler();
    int SdMonitorStart();
    int SdMonitorStop();
    int PrintInfo() const;

public:
    SdmmcService(const std::string& disk_name, const std::string& mount_point);

    bool Initialize() override;
};

}
