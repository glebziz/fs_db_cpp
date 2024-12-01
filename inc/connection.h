#ifndef FS_DB_CPP_CONNECTION_H
#define FS_DB_CPP_CONNECTION_H

#include "iso_level.h"
#include "exception.h"

#ifdef __cplusplus

#include <string>

#include "store.h"

namespace fs_db {
    class Transaction;

    class Connection final : Store {
        friend class Transaction;
    public:
        Connection() = default;
        explicit Connection(const std::string& url);

        void Connect(const std::string& url);

        void Set(const std::string &key, const std::string &content) const override;
        [[nodiscard]] WriteFile Set(const std::string &key) const override;
        [[nodiscard]] std::string Get(const std::string &key) const override;
        [[nodiscard]] ReadFile GetFile(const std::string &key) const override;
        [[nodiscard]] std::vector<std::string> GetKeys() const override;
        void Delete(const std::string &key) const override;

        [[nodiscard]] Transaction Begin(TxIsoLevel isoLevel) const;
    };
}

typedef fs_db::Connection fs_db_conn;
typedef fs_db::Transaction fs_db_tx;

#else

typedef struct fs_db_conn fs_db_conn;
typedef struct fs_db_tx fs_db_tx;

#endif

#ifdef __cplusplus
extern "C" {
#endif
    fs_db_conn *fs_db_connect(const char *url);
    fs_db_err fs_db_begin(const fs_db_conn *conn, fs_db_iso_level iso_level, fs_db_tx **tx);
    void fs_db_conn_destroy(fs_db_conn **conn);
#ifdef __cplusplus
}
#endif

#endif //FS_DB_CPP_CONNECTION_H
