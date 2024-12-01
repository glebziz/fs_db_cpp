#include "inc/exception.h"

#include <grpcpp/grpcpp.h>

#include "error.pb.h"

const std::string messages[] = {
    "",                             // NO_ERR
    "unknown error",                // UNKNOWN_ERR
    "connection not initialized",   // NOT_INIT_ERR
    "no free space",                // NO_FREE_SPACE_ERR
    "not found",                    // NOT_FOUND_ERR
    "empty key",                    // EMPTY_KEY_ERR
    "header not found",             // HEADER_NOT_FOUND_ERR
    "transaction not found",        // TX_NOT_FOUND_ERR
    "transaction already exists",   // TX_ALREADY_EXISTS_ERR
    "serialization error"           // TX_SERIALIZATION_ERR
};

fs_db::Exception::Exception(): Exception(UNKNOWN_ERR) {}

fs_db::Exception::Exception(const ErrCode code): code(code) {
    msg = fs_db_err_msg(code);
}

fs_db::Exception::Exception(std::string msg): msg(std::move(msg)), code(UNKNOWN_ERR) {}

const char *fs_db::Exception::what() {
    return msg.data();
}

const char *fs_db_err_msg(const fs_db_err code) {
    return messages[code].data();
}

fs_db::ErrCode fs_db::Exception::try_catch(const std::function<void()> &fn) {
    try {
        fn();
    } catch (const Exception &ex) {
        return ex.code;
    } catch (...) {
        return UNKNOWN_ERR;
    }

    return NO_ERR;
}

void handleProtoError(const grpc::Status &st) {
    size_t pos;
    store::Error err;
    auto details = st.error_details();

    while ((pos = details.find('\b', 1)) != std::string::npos) {
        details = details.substr(pos);

        if (!err.ParsePartialFromString(details)) {
            continue;
        }

        switch (err.code()) {
            case store::ErrNoFreeSpace:
                throw fs_db::Exception(fs_db::ErrCode::NO_FREE_SPACE_ERR);
            case store::ErrNotFound:
                throw fs_db::Exception(fs_db::ErrCode::NOT_FOUND_ERR);
            case store::ErrEmptyKey:
                throw fs_db::Exception(fs_db::ErrCode::EMPTY_KEY_ERR);
            case store::ErrHeaderNotFound:
                throw fs_db::Exception(fs_db::ErrCode::HEADER_NOT_FOUND_ERR);
            case store::ErrTxNotFound:
                throw fs_db::Exception(fs_db::ErrCode::TX_NOT_FOUND_ERR);
            case store::ErrTxAlreadyExists:
                throw fs_db::Exception(fs_db::ErrCode::TX_ALREADY_EXISTS_ERR);
            case store::ErrTxSerialization:
                throw fs_db::Exception(fs_db::ErrCode::TX_SERIALIZATION_ERR);
            case store::ErrUnknown:
                throw fs_db::Exception(fs_db::ErrCode::UNKNOWN_ERR);
            default:
                break;
        }
    }
}

void handleError(const grpc::Status &st) {
    if (st.ok()) {
        return;
    }

    handleProtoError(st);
    switch (st.error_code()) {
        case grpc::StatusCode::INVALID_ARGUMENT:
            throw fs_db::Exception(fs_db::ErrCode::EMPTY_KEY_ERR);
        case grpc::StatusCode::NOT_FOUND:
            throw fs_db::Exception(fs_db::ErrCode::NOT_FOUND_ERR);
        case grpc::StatusCode::ALREADY_EXISTS:
            throw fs_db::Exception(fs_db::ErrCode::TX_ALREADY_EXISTS_ERR);
        case grpc::StatusCode::RESOURCE_EXHAUSTED:
            throw fs_db::Exception(fs_db::ErrCode::NO_FREE_SPACE_ERR);
        case grpc::StatusCode::FAILED_PRECONDITION:
            throw fs_db::Exception(fs_db::ErrCode::TX_SERIALIZATION_ERR);
        case grpc::StatusCode::ABORTED:
            throw fs_db::Exception(fs_db::ErrCode::TX_NOT_FOUND_ERR);
        default:
            throw fs_db::Exception(st.error_message());
    }
}