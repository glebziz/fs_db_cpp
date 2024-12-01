#include "inc/connection.h"
#include "inc/transaction.h"
#include "inc/write_file.h"
#include "inc/read_file.h"

void handleError(const grpc::Status &st);
store::TxIsoLevel txIsoLevelToPb(fs_db_iso_level isoLevel);

fs_db::Connection::Connection(const std::string& url): Store(url) {}

void fs_db::Connection::Connect(const std::string& url) {
    connect(url);
}

void fs_db::Connection::Set(const std::string &key, const std::string &content) const {
    grpc::ClientContext ctx;
    _set(ctx, key, content);
}

fs_db::WriteFile fs_db::Connection::Set(const std::string &key) const {
    return _set(std::make_unique<grpc::ClientContext>(), key);
}

std::string fs_db::Connection::Get(const std::string &key) const {
    grpc::ClientContext ctx;
    return _get(ctx, key);
}

fs_db::ReadFile fs_db::Connection::GetFile(const std::string &key) const {
    return _get(std::make_unique<grpc::ClientContext>(), key);
}

std::vector<std::string> fs_db::Connection::GetKeys() const {
    grpc::ClientContext ctx;
    return _get_keys(ctx);
}


void fs_db::Connection::Delete(const std::string &key) const {
    grpc::ClientContext ctx;
    _delete(ctx, key);
}

fs_db::Transaction fs_db::Connection::Begin(const TxIsoLevel isoLevel) const {
    if (!stub) {
        throw Exception(NOT_INIT_ERR);
    }

    grpc::ClientContext ctx;
    store::BeginTxRequest req;
    store::BeginTxResponse resp;

    req.set_isolevel(txIsoLevelToPb(isoLevel));
    const auto st = stub->BeginTx(&ctx, req, &resp);
    handleError(st);

    return Transaction(*this, resp.id());
}


fs_db_conn *fs_db_connect(const char *url) {
    return new fs_db::Connection(url);
}

fs_db_err fs_db_begin(const fs_db_conn *conn, const fs_db_iso_level iso_level, fs_db_tx **tx) {
    return fs_db::Exception::try_catch([&] {
        *tx = new fs_db::Transaction(std::move(conn->Begin(iso_level)));
    });
}

void fs_db_conn_destroy(fs_db_conn **conn) {
    delete *conn;
    *conn = nullptr;
}
