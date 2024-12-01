#ifndef FS_DB_CPP_EXCEPTION_H
#define FS_DB_CPP_EXCEPTION_H

#ifdef __cplusplus

#include <functional>
#include <exception>
#include <string>

namespace fs_db {
    enum ErrCode {
        NO_ERR,
        UNKNOWN_ERR,
        NOT_INIT_ERR,
        NO_FREE_SPACE_ERR,
        NOT_FOUND_ERR,
        EMPTY_KEY_ERR,
        HEADER_NOT_FOUND_ERR,
        TX_NOT_FOUND_ERR,
        TX_ALREADY_EXISTS_ERR,
        TX_SERIALIZATION_ERR
    };

    class Exception final : std::exception {
        std::string msg;

    public:
        ErrCode code;

        explicit Exception();
        explicit Exception(ErrCode code);
        explicit Exception(std::string msg);

        const char *what();

        static ErrCode try_catch(const std::function<void()> &fn);
    };
}

typedef fs_db::ErrCode fs_db_err;

#else

typedef enum fs_db_err {
    NO_ERR,
    UNKNOWN_ERR,
    NOT_INIT_ERR,
    NO_FREE_SPACE_ERR,
    NOT_FOUND_ERR,
    EMPTY_KEY_ERR,
    HEADER_NOT_FOUND_ERR,
    TX_NOT_FOUND_ERR,
    TX_ALREADY_EXISTS_ERR,
    TX_SERIALIZATION_ERR
} fs_db_err;

#endif

#ifdef __cplusplus
extern "C" {
#endif
    const char *fs_db_err_msg(fs_db_err code);
#ifdef __cplusplus
}
#endif

#endif //FS_DB_CPP_EXCEPTION_H
