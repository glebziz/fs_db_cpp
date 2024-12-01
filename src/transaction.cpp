#include "inc/transaction.h"
#include "inc/connection.h"
#include "inc/write_file.h"
#include "inc/read_file.h"

static const std::string txIdKey = "mdtxidkey";

void handleError(const grpc::Status &st);

fs_db::Transaction::Transaction(const Store &store, const std::string &id): Store(store), id(std::make_unique<std::string>(id)) {}

fs_db::Transaction::Transaction(Transaction &&other) noexcept: Store(other), id(std::move(other.id)) {}

fs_db::Transaction::~Transaction() {
    Rollback();
}

void fs_db::Transaction::Set(const std::string &key, const std::string &content) const {
    if (id == nullptr) {
        throw Exception(NOT_INIT_ERR);
    }

    grpc::ClientContext ctx;
    ctx.AddMetadata(txIdKey, *id);
    _set(ctx, key, content);
}

fs_db::WriteFile fs_db::Transaction::Set(const std::string &key) const {
    if (id == nullptr) {
        throw Exception(NOT_INIT_ERR);
    }

    auto ctx = std::make_unique<grpc::ClientContext>();
    ctx->AddMetadata(txIdKey, *id);
    return _set(std::move(ctx), key);
}

std::string fs_db::Transaction::Get(const std::string &key) const {
    if (id == nullptr) {
        throw Exception(NOT_INIT_ERR);
    }

    grpc::ClientContext ctx;
    ctx.AddMetadata(txIdKey, *id);
    return _get(ctx, key);
}

fs_db::ReadFile fs_db::Transaction::GetFile(const std::string &key) const {
    if (id == nullptr) {
        throw Exception(NOT_INIT_ERR);
    }

    auto ctx = std::make_unique<grpc::ClientContext>();
    ctx->AddMetadata(txIdKey, *id);
    return _get(std::move(ctx), key);
}

std::vector<std::string> fs_db::Transaction::GetKeys() const {
    if (id == nullptr) {
        throw Exception(NOT_INIT_ERR);
    }

    grpc::ClientContext ctx;
    ctx.AddMetadata(txIdKey, *id);
    return _get_keys(ctx);
}

void fs_db::Transaction::Delete(const std::string &key) const {
    if (id == nullptr) {
        throw Exception(NOT_INIT_ERR);
    }

    grpc::ClientContext ctx;
    ctx.AddMetadata(txIdKey, *id);
    _delete(ctx, key);
}

void fs_db::Transaction::Commit() const {
    if (id == nullptr) {
        return;
    }

    if (!stub) {
        throw Exception(NOT_INIT_ERR);
    }

    grpc::ClientContext ctx;
    ctx.AddMetadata(txIdKey, *id);

    const store::CommitTxRequest req;
    store::CommitTxResponse resp;

    const auto st = stub->CommitTx(&ctx, req, &resp);
    handleError(st);
}

void fs_db::Transaction::Rollback() const {
    if (id == nullptr) {
        return;
    }

    if (!stub) {
        throw Exception(NOT_INIT_ERR);
    }

    grpc::ClientContext ctx;
    ctx.AddMetadata(txIdKey, *id);

    const store::RollbackTxRequest req;
    store::RollbackTxResponse resp;

    const auto st = stub->RollbackTx(&ctx, req, &resp);
    handleError(st);
}

fs_db_err fs_db_tx_commit(fs_db_tx **tx) {
    const fs_db_err err = fs_db::Exception::try_catch([&] {
        (*tx)->Commit();
    });

    delete *tx;
    *tx = nullptr;

    return err;
}

fs_db_err fs_db_tx_rollback(fs_db_tx **tx) {
    const fs_db_err err = fs_db::Exception::try_catch([&] {
        (*tx)->Rollback();
    });

    delete *tx;
    *tx = nullptr;

    return err;
}
