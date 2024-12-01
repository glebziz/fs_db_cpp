#include "inc/store.h"
#include "inc/write_file.h"
#include "inc/read_file.h"

void handleError(const grpc::Status &st);

fs_db::Store::Store(const std::string& url) {
    connect(url);
}

void fs_db::Store::connect(const std::string& url) {
    if (stub) {
        return;
    }

    stub = store::StoreV1::NewStub(
            CreateChannel(
                    url, grpc::InsecureChannelCredentials()));
}

void fs_db::Store::_set(grpc::ClientContext &ctx, const std::string &key, const std::string &content) const {
    if (!stub) {
        throw Exception(NOT_INIT_ERR);
    }

    store::SetFileRequest msg;
    store::SetFileResponse resp;

    auto writer = stub->SetFile(&ctx, &resp);

    store::FileHeader h;
    h.set_key(key);
    msg.set_allocated_header(&h);

    if (!writer->Write(msg)) {
        handleError(writer->Finish());

        return;
    }

    msg.release_header();

    WriteFile wf(nullptr, std::move(writer), nullptr);
    wf.Write(content);
    wf.Close();
}

fs_db::WriteFile fs_db::Store::_set(std::unique_ptr<grpc::ClientContext> &&ctx, const std::string &key) const {
    if (!stub) {
        throw Exception(NOT_INIT_ERR);
    }

    store::SetFileRequest msg;
    auto resp = std::make_unique<store::SetFileResponse>();

    auto writer = stub->SetFile(ctx.get(), resp.get());

    store::FileHeader h;
    h.set_key(key);
    msg.set_allocated_header(&h);

    if (!writer->Write(msg)) {
        handleError(writer->Finish());

        return {};
    }

    msg.release_header();

    return WriteFile(std::move(ctx), std::move(writer), std::move(resp));
}

std::string fs_db::Store::_get(grpc::ClientContext &ctx, const std::string &key) const {
    if (!stub) {
        throw Exception(NOT_INIT_ERR);
    }

    store::GetFileRequest req;
    store::GetFileResponse msg;

    req.set_key(key);
    const auto reader = stub->GetFile(&ctx, req);

    if (!reader->Read(&msg)) {
        handleError(reader->Finish());

        return {};
    }

    std::string out;
    while (reader->Read(&msg)) {
        out += msg.chunk();
    }

    handleError(reader->Finish());

    return out;
}

fs_db::ReadFile fs_db::Store::_get(std::unique_ptr<grpc::ClientContext> &&ctx, const std::string &key) const {
    if (!stub) {
        throw Exception(NOT_INIT_ERR);
    }

    store::GetFileRequest req;
    store::GetFileResponse msg;

    req.set_key(key);
    auto reader = stub->GetFile(ctx.get(), req);

    if (!reader->Read(&msg)) {
        handleError(reader->Finish());

        return {};
    }

    return ReadFile(std::move(ctx), std::move(reader));
}

std::vector<std::string> fs_db::Store::_get_keys(grpc::ClientContext &ctx) const {
    if (!stub) {
        throw Exception(NOT_INIT_ERR);
    }

    const store::GetKeysRequest req;
    store::GetKeysResponse resp;

    const auto st = stub->GetKeys(&ctx, req, &resp);
    handleError(st);

    return {resp.keys().begin(), resp.keys().end()};
}

void fs_db::Store::_delete(grpc::ClientContext &ctx, const std::string &key) const {
    if (!stub) {
        throw Exception(NOT_INIT_ERR);
    }

    store::DeleteFileRequest req;
    store::DeleteFileResponse resp;

    req.set_key(key);
    const auto st = stub->DeleteFile(&ctx, req, &resp);
    handleError(st);
}

fs_db_err fs_db_set(const fs_db_store *store, const char *key, const char *content, const size_t size) {
    return fs_db::Exception::try_catch([&] {
        const std::string str(content, size);
        store->Set(key, str);
    });
}

fs_db_err fs_db_set_file(const fs_db_store *store, const char *key, fs_db_wf **wf) {
    return fs_db::Exception::try_catch([&] {
        *wf = new fs_db::WriteFile(store->Set(key));
    });
}

fs_db_err fs_db_get(const fs_db_store *store, const char *key, char **content, size_t *size) {
    return fs_db::Exception::try_catch([&] {
        auto str  = store->Get(key);

        *content = new char[str.size()];
        *size = str.size();
        std::copy(str.begin(), str.end(), *content);
    });
}

fs_db_err fs_db_get_file(const fs_db_store *store, const char *key, fs_db_rf **rf) {
    return fs_db::Exception::try_catch([&] {
        *rf = new fs_db::ReadFile(store->GetFile(key));
    });
}

fs_db_err fs_db_delete(const fs_db_store *store, const char *key) {
    return fs_db::Exception::try_catch([&] {
        store->Delete(key);
    });
}
