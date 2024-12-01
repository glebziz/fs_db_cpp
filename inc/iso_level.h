#ifndef TX_LEVEL_H
#define TX_LEVEL_H

#ifdef __cplusplus

namespace fs_db {
    enum TxIsoLevel {
        ISO_LEVEL_READ_UNCOMMITTED,
        ISO_LEVEL_READ_COMMITTED,
        ISO_LEVEL_REPEATABLE_READ,
        ISO_LEVEL_SERIALIZABLE,
        ISO_LEVEL_DEFAULT = ISO_LEVEL_READ_COMMITTED
    };
}

typedef fs_db::TxIsoLevel fs_db_iso_level;

#else

typedef enum fs_db_iso_level {
    ISO_LEVEL_READ_UNCOMMITTED,
    ISO_LEVEL_READ_COMMITTED,
    ISO_LEVEL_REPEATABLE_READ,
    ISO_LEVEL_SERIALIZABLE,
    ISO_LEVEL_DEFAULT = ISO_LEVEL_READ_COMMITTED
} fs_db_iso_level;

#endif

#endif //TX_LEVEL_H
