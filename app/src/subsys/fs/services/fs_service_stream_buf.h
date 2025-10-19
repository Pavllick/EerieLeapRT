#pragma once

#include <streambuf>
#include <filesystem>
#include <string>

#include "i_fs_service.h"

namespace eerie_leap::subsys::fs::services {

class FsServiceStreamBuf : public std::streambuf {
private:
    IFsService* fs_service_;
    std::string relative_path_;
    struct fs_file_t file_;
    bool file_opened_;

protected:
    std::streamsize xsputn(const char* s, std::streamsize n) override;
    std::streambuf::int_type overflow(std::streambuf::int_type c) override;
    int sync() override;

public:
    FsServiceStreamBuf(IFsService* fs_service, const std::string& relative_path, bool append = false);
    ~FsServiceStreamBuf();

    bool close();
    bool is_open() const;

    FsServiceStreamBuf(const FsServiceStreamBuf&) = delete;
    FsServiceStreamBuf& operator=(const FsServiceStreamBuf&) = delete;
};

} // namespace eerie_leap::subsys::fs::services
