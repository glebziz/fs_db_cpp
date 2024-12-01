#ifndef FILE_H
#define FILE_H

#include "exception.h"

#ifdef __cplusplus

#include <memory>

#include "store_service.pb.h"
#include "store_service.grpc.pb.h"

namespace fs_db {
    class Store;

    class WriteFile {
        friend class Store;

        std::string buf;
        std::unique_ptr<grpc::ClientContext> ctx;
        std::unique_ptr<store::SetFileResponse> resp;
        std::unique_ptr<grpc::ClientWriter<store::SetFileRequest>> w;

        WriteFile() = default;
        explicit WriteFile(std::unique_ptr<grpc::ClientContext> &&ctx, std::unique_ptr<grpc::ClientWriter<store::SetFileRequest>> &&w, std::unique_ptr<store::SetFileResponse> &&resp);

    public:
        WriteFile(WriteFile &&other) noexcept;
        ~WriteFile();

        void Write(const std::string &data);
        void Close();

        WriteFile &operator=(WriteFile &&) noexcept;
    };
}

typedef fs_db::WriteFile fs_db_wf;

#else

typedef struct fs_db_wf fs_db_wf;

#endif

#ifdef __cplusplus
extern "C" {
#endif
    fs_db_err fs_db_wf_write(fs_db_wf *wf, const char *data, size_t size);
    fs_db_err fs_db_wf_close(fs_db_wf **wf);
#ifdef __cplusplus
}
#endif

#endif //FILE_H
