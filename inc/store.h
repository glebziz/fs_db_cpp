#ifndef STORE_H
#define STORE_H

#include "exception.h"

#ifdef __cplusplus

#include <grpcpp/grpcpp.h>

#include "store_service.grpc.pb.h"

namespace fs_db {
    class WriteFile;
    class ReadFile;

    class Store {
    protected:
        std::shared_ptr<store::StoreV1::Stub> stub;

        Store() = default;
        explicit Store(const std::string& url);
        Store(const Store& other) = default;

        void connect(const std::string& url);

        void _set(grpc::ClientContext &ctx, const std::string &key, const std::string &content) const;
        WriteFile _set(std::unique_ptr<grpc::ClientContext> &&ctx, const std::string &key) const;
        std::string _get(grpc::ClientContext &ctx, const std::string &key) const;
        ReadFile _get(std::unique_ptr<grpc::ClientContext> &&ctx, const std::string &key) const;
        std::vector<std::string> _get_keys(grpc::ClientContext &ctx) const;
        void _delete(grpc::ClientContext &ctx, const std::string &key) const;

    public:
        virtual ~Store() = default;

        virtual void Set(const std::string &key, const std::string &content) const = 0;
        [[nodiscard]] virtual WriteFile Set(const std::string &key) const = 0;
        [[nodiscard]] virtual std::string Get(const std::string &key) const = 0;
        [[nodiscard]] virtual ReadFile GetFile(const std::string &key) const = 0;
        [[nodiscard]] virtual std::vector<std::string> GetKeys() const = 0;
        virtual void Delete(const std::string &key) const = 0;
    };
}

typedef fs_db::Store fs_db_store;
typedef fs_db::WriteFile fs_db_wf;
typedef fs_db::ReadFile fs_db_rf;

#else

typedef struct fs_db_store fs_db_store;
typedef struct fs_db_wf fs_db_wf;
typedef struct fs_db_rf fs_db_rf;

#endif

#ifdef __cplusplus
extern "C" {
#endif
    fs_db_err fs_db_set(const fs_db_store *store, const char *key, const char *content, size_t size);
    fs_db_err fs_db_set_file(const fs_db_store *store, const char *key, fs_db_wf **wf);
    fs_db_err fs_db_get(const fs_db_store *store, const char *key, char **content, size_t *size);
    fs_db_err fs_db_get_file(const fs_db_store *store, const char *key, fs_db_rf **rf);
    fs_db_err fs_db_delete(const fs_db_store *store, const char *key);
#ifdef __cplusplus
}
#endif

#endif //STORE_H
