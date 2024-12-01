#ifndef TRANSACTION_H
#define TRANSACTION_H

#include "exception.h"

#ifdef __cplusplus

#include <string>
#include <vector>

#include "store.h"

namespace fs_db {
    class Connection;

    class Transaction final: Store {
        friend class Connection;

        std::unique_ptr<std::string> id;

        explicit Transaction(const Store &store, const std::string &id);
    public:
        Transaction(Transaction &&other) noexcept;
        ~Transaction() override;

        void Set(const std::string &key, const std::string &content) const override;
        [[nodiscard]] WriteFile Set(const std::string &key) const override;
        [[nodiscard]] std::string Get(const std::string &key) const override;
        [[nodiscard]] ReadFile GetFile(const std::string &key) const override;
        [[nodiscard]] std::vector<std::string> GetKeys() const override;
        void Delete(const std::string &key) const override;

        void Commit() const;
        void Rollback() const;
    };
}

typedef fs_db::Transaction fs_db_tx;

#else

typedef struct fs_db_tx fs_db_tx;

#endif

#ifdef __cplusplus
extern "C" {
#endif
    fs_db_err fs_db_tx_commit(fs_db_tx **tx);
    fs_db_err fs_db_tx_rollback(fs_db_tx **tx);
#ifdef __cplusplus
}
#endif

#endif //TRANSACTION_H
