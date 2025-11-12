#pragma once

#include <string>
#include <vector>
#include <zephyr/device.h>
#include <zephyr/fs/fs.h>
#include "i_fs_service.h"

namespace eerie_leap::subsys::fs::services {

class FsService : public IFsService {
protected:
    struct fs_mount_t mountpoint_;

    FsService() = default;

    bool Mount();
    void Unmount();
    bool IsMounted() const;

public:
    FsService(fs_mount_t mountpoint);

    bool Initialize() override;
    bool IsAvailable() const override;
    bool WriteFile(std::string_view relative_path, const void* data_p, size_t data_size, bool append = false) override;
    bool ReadFile(std::string_view relative_path, void* data_p, size_t data_size, size_t& out_len) override;
    bool CreateDirectory(std::string_view relative_path) override;
    bool Exists(std::string_view relative_path) override;
    bool DeleteFile(std::string_view relative_path) override;
    bool DeleteRecursive(std::string_view relative_path = "") override;
    std::vector<std::string> ListFiles(std::string_view relative_path = "") const override;
    size_t GetFileSize(std::string_view relative_path) const override;
    size_t GetTotalSpace() const override;
    size_t GetUsedSpace() const override;
    bool Format() override;
    const fs_mount_t& GetMountpoint() const override;
};

} // namespace eerie_leap::subsys::fs::services
