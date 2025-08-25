#include <filesystem>

#include <zephyr/logging/log.h>

#include "dt_fs.h"

namespace eerie_leap::subsys::device_tree {

LOG_MODULE_REGISTER(dt_fs_logger);

std::optional<fs_mount_t> DtFs::int_fs_mp_ = std::nullopt;
std::optional<fs_mount_t> DtFs::sd_fs_mp_ = std::nullopt;

void DtFs::InitInternalFs() {
#if DT_HAS_ALIAS(intfs0)
    int_fs_mp_ = std::make_optional<fs_mount_t>(FS_FSTAB_ENTRY(INT_FS_NODE));
    LOG_INF("Internal File System initialized.");
#endif
}

void DtFs::InitSdFs() {
#if DT_HAS_ALIAS(sdhc0) && DT_HAS_ALIAS(sdfs0)
    sd_fs_mp_ = std::make_optional<fs_mount_t>(FS_FSTAB_ENTRY(SD_FS_NODE));
    sd_fs_mp_.value().storage_dev = (void *)SD_DEV;

    LOG_INF("SD File System initialized.");
#endif
}

} // namespace eerie_leap::subsys::device_tree
