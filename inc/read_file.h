#ifndef READ_FILE_H
#define READ_FILE_H

#include "exception.h"

#ifdef __cplusplus

#include <memory>
#include <string>

#include "store_service.pb.h"
#include "store_service.grpc.pb.h"

namespace fs_db {
    class Store;

    class ReadFile {
        friend class Store;

        std::string buf;
        std::unique_ptr<grpc::ClientContext> ctx;
        std::unique_ptr<grpc::ClientReader<store::GetFileResponse>> r;

        ReadFile() = default;
        explicit ReadFile(std::unique_ptr<grpc::ClientContext> &&ctx, std::unique_ptr<grpc::ClientReader<store::GetFileResponse>> &&r);

    public:
        ReadFile(ReadFile &&other) noexcept;
        ~ReadFile();

        size_t Read(char data[], size_t size);
        void Close();

        ReadFile &operator=(ReadFile &&other) noexcept;
    };
}

typedef fs_db::ReadFile fs_db_rf;

#else

typedef struct fs_db_rf fs_db_rf;

#endif

#ifdef __cplusplus
extern "C" {
#endif
    fs_db_err fs_db_rf_read(fs_db_rf *rf, char data[], size_t size, size_t *read);
    fs_db_err fs_db_rf_close(fs_db_rf **rf);
#ifdef __cplusplus
}
#endif

#endif //READ_FILE_H
