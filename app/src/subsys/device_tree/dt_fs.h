#pragma once

#include <vector>
#include <optional>

#include <zephyr/devicetree.h>
#include <zephyr/device.h>
#include <zephyr/fs/fs.h>

// Internal File System
#if DT_HAS_ALIAS(intfs0)
#define INT_FS_NODE DT_ALIAS(intfs0)
FS_FSTAB_DECLARE_ENTRY(INT_FS_NODE);
#endif

// SD File System
#if DT_HAS_ALIAS(sdhc0) && DT_HAS_ALIAS(sdfs0)
#define SD_DEV DEVICE_DT_GET(DT_ALIAS(sdhc0))
#define SD_FS_NODE DT_ALIAS(sdfs0)
FS_FSTAB_DECLARE_ENTRY(SD_FS_NODE);
#endif

namespace eerie_leap::subsys::device_tree {

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

} // namespace eerie_leap::subsys::device_tree
