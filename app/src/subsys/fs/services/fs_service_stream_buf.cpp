#include "fs_service_stream_buf.h"

namespace eerie_leap::subsys::fs::services {

FsServiceStreamBuf::FsServiceStreamBuf(IFsService* fs_service, const std::string& relative_path, OpenMode mode)
    : fs_service_(fs_service), relative_path_(relative_path), file_opened_(false), input_buffer_(BUFFER_SIZE) {

    if(!fs_service_)
        throw std::invalid_argument("fs_service cannot be null");

    if(!fs_service_->IsAvailable())
        throw std::runtime_error("Filesystem not available");

    std::filesystem::path full_path(fs_service_->GetMountpoint().mnt_point);
    full_path /= relative_path_;

    if(mode == OpenMode::Write || mode == OpenMode::Append) {
        auto parent = std::filesystem::path(relative_path_).parent_path();
        if(!fs_service_->Exists(parent.string()) && !parent.empty())
            fs_service_->CreateDirectory(parent.string());
    }

    fs_mode_t open_mode = 0;
    switch(mode) {
        case OpenMode::Read:
            open_mode = FS_O_READ;
            break;
        case OpenMode::Write:
            open_mode = FS_O_WRITE | FS_O_CREATE | FS_O_TRUNC;
            break;
        case OpenMode::Append:
            open_mode = FS_O_WRITE | FS_O_CREATE | FS_O_APPEND;
            break;
    }

    fs_file_t_init(&file_);
    int rc = fs_open(&file_, full_path.string().c_str(), open_mode);

    if(rc < 0)
        throw std::runtime_error("Failed to open file: " + std::to_string(rc));

    file_opened_ = true;

    setg(nullptr, nullptr, nullptr);
}

FsServiceStreamBuf::~FsServiceStreamBuf() {
    close();
}

bool FsServiceStreamBuf::close() {
    if(file_opened_) {
        int rc = fs_close(&file_);
        file_opened_ = false;
        return rc == 0;
    }

    return true;
}

bool FsServiceStreamBuf::is_open() const {
    return file_opened_;
}

std::streamsize FsServiceStreamBuf::xsputn(const char* s, std::streamsize n) {
    if(!file_opened_)
        return 0;

    ssize_t rc = fs_write(&file_, s, static_cast<size_t>(n));
    if(rc < 0)
        return 0;

    return rc;
}

std::streambuf::int_type FsServiceStreamBuf::overflow(std::streambuf::int_type c) {
    if(c != traits_type::eof()) {
        char ch = traits_type::to_char_type(c);
        if(xsputn(&ch, 1) == 1)
            return c;
    }

    return traits_type::eof();
}

std::streamsize FsServiceStreamBuf::xsgetn(char* s, std::streamsize n) {
    if(!file_opened_)
        return 0;

    ssize_t rc = fs_read(&file_, s, static_cast<size_t>(n));
    if(rc < 0)
        return 0;

    return rc;
}

std::streambuf::int_type FsServiceStreamBuf::underflow() {
    if(!file_opened_)
        return traits_type::eof();

    if(gptr() < egptr())
        return traits_type::to_int_type(*gptr());

    ssize_t bytes_read = fs_read(&file_, input_buffer_.data(), input_buffer_.size());

    if(bytes_read <= 0)
        return traits_type::eof();

    setg(input_buffer_.data(), input_buffer_.data(), input_buffer_.data() + bytes_read);

    return traits_type::to_int_type(*gptr());
}

int FsServiceStreamBuf::sync() {
    if(file_opened_)
        return fs_sync(&file_);

    return -1;
}

} // namespace eerie_leap::subsys::fs::services
