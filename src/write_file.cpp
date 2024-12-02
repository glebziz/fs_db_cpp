#include "inc/write_file.h"

void handleError(const grpc::Status &st);

fs_db::WriteFile::WriteFile(WriteFile &&other) noexcept: ctx(std::move(other.ctx)), resp(std::move(other.resp)), w(std::move(other.w)) {}

fs_db::WriteFile::WriteFile(std::unique_ptr<grpc::ClientContext> &&ctx, std::unique_ptr<grpc::ClientWriter<store::SetFileRequest>> &&w, std::unique_ptr<store::SetFileResponse> &&resp): ctx(std::move(ctx)), resp(std::move(resp)), w(std::move(w)) {}

fs_db::WriteFile::~WriteFile() {
    Close();
}

void fs_db::WriteFile::Write(const std::string &data) {
    if (w == nullptr) {
        return;
    }

    buf += data;
    store::SetFileRequest msg;
    while (buf.size() >= store::ChunkSize_MAX) {
        msg.set_chunk(buf.substr(0, store::ChunkSize_MAX));

        if (!w->Write(msg)) {
            handleError(w->Finish());

            return;
        }

        buf = buf.substr(store::ChunkSize_MAX);
    }
}

void fs_db::WriteFile::Close() {
    if (w == nullptr) {
        return;
    }

    if (!buf.empty()) {
        store::SetFileRequest msg;
        msg.set_chunk(buf);

        if (!w->Write(msg)) {
            handleError(w->Finish());
            return;
        }
    }

    w->WritesDone();
    handleError(w->Finish());
    ctx.reset();
    resp.reset();
    w.reset();
}

fs_db::WriteFile &fs_db::WriteFile::operator=(WriteFile &&other) noexcept {
    buf = std::move(other.buf);
    ctx = std::move(other.ctx);
    resp = std::move(other.resp);
    w = std::move(other.w);

    return *this;
}

fs_db_err fs_db_wf_write(fs_db_wf *wf, const char *data, const size_t size) {
    return fs_db::Exception::try_catch([&] {
        wf->Write(std::string(data, size));
    });
}

fs_db_err fs_db_wf_close(fs_db_wf **wf) {
    const fs_db_err err = fs_db::Exception::try_catch([&] {
        (*wf)->Close();
    });

    delete *wf;
    *wf = nullptr;

    return err;
}
