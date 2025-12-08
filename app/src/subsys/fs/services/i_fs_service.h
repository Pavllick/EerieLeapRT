#pragma once

#include <cstddef>
#include <string>
#include <vector>

#include "zephyr/fs/fs.h"

namespace eerie_leap::subsys::fs::services {

class IFsService {
public:
    virtual ~IFsService() = default;

    virtual bool Initialize() = 0;
    virtual bool IsAvailable() const = 0;
    virtual bool WriteFile(std::string_view relative_path, const void* data_p, size_t data_size, bool append = false) = 0;
    virtual bool ReadFile(std::string_view relative_path, void* data_p, size_t data_size, size_t& out_len) = 0;
    virtual bool CreateDirectory(std::string_view relative_path) = 0;
    virtual bool Exists(std::string_view relative_path) = 0;
    virtual bool DeleteFile(std::string_view relative_path) = 0;
    virtual bool DeleteRecursive(std::string_view relative_path = "") = 0;
    virtual std::vector<std::string> ListFiles(std::string_view relative_path = "") const = 0;
    virtual size_t GetFileSize(std::string_view relative_path) const = 0;
    virtual size_t GetTotalSpace() const = 0;
    virtual size_t GetUsedSpace() const = 0;
    virtual bool Format() = 0;
    virtual const fs_mount_t& GetMountpoint() const = 0;
};

} // namespace eerie_leap::subsys::fs::services
