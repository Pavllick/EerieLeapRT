#pragma once

#include <vector>
#include <optional>

#include <zephyr/devicetree.h>
#include <zephyr/device.h>
#include <zephyr/fs/fs.h>

// File System
#define INT_FS_NODE DT_ALIAS(fs0)
FS_FSTAB_DECLARE_ENTRY(INT_FS_NODE);

namespace eerie_leap::domain::device_tree {

class DtFs {
private:
    static std::optional<fs_mount_t> int_fs_mp_;
    static std::optional<fs_mount_t> sd_fs_mp_;

    DtFs() = default;

public:
    static void InitInternalFs();
    static void InitSdFs();

    static std::optional<fs_mount_t>& GetInternalFsMp() { return int_fs_mp_; }
    static std::optional<fs_mount_t>& GetSdFsMp() { return sd_fs_mp_; }
};

} // namespace eerie_leap::domain::device_tree
