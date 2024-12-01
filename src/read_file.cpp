#include "inc/read_file.h"

void handleError(const grpc::Status &st);

fs_db::ReadFile::ReadFile(ReadFile &&other) noexcept: buf(std::move(other.buf)), ctx(std::move(other.ctx)), r(std::move(other.r)) {}

fs_db::ReadFile::ReadFile(std::unique_ptr<grpc::ClientContext> &&ctx, std::unique_ptr<grpc::ClientReader<store::GetFileResponse>> &&r): ctx(std::move(ctx)), r(std::move(r)) {}

size_t fs_db::ReadFile::Read(char data[], size_t size) {
    if (r == nullptr) {
        return 0;
    }

    store::GetFileResponse msg;
    while (buf.size() < size && r->Read(&msg)) {
        handleError(r->Finish());
        buf += msg.chunk();
    }

    size = std::min(buf.size(), size);
    std::copy_n(buf.begin(), size, data);
    buf = std::string(buf.begin() + static_cast<long>(size), buf.end());

    return size;
}

fs_db::ReadFile &fs_db::ReadFile::operator=(ReadFile &&other) noexcept {
    buf = std::move(other.buf);
    ctx = std::move(other.ctx);
    r = std::move(other.r);

    return *this;
}

fs_db_err fs_db_rf_read(fs_db_rf *rf, char data[], const size_t size, size_t *read) {
    return fs_db::Exception::try_catch([&] {
        *read = rf->Read(data, size);
    });
}

void fs_db_rf_destroy(fs_db_rf **rf) {
    delete *rf;
    *rf = nullptr;
}