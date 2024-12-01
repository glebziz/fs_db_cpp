#include "inc/iso_level.h"

#include "store_service.pb.h"

store::TxIsoLevel txIsoLevelToPb(const fs_db_iso_level isoLevel) {
    switch (isoLevel) {
        case fs_db::ISO_LEVEL_READ_UNCOMMITTED:
            return store::TxIsoLevel::ISO_LEVEL_READ_UNCOMMITTED;
        case fs_db::ISO_LEVEL_READ_COMMITTED:
            return store::TxIsoLevel::ISO_LEVEL_READ_COMMITTED;
        case fs_db::ISO_LEVEL_REPEATABLE_READ:
            return store::TxIsoLevel::ISO_LEVEL_REPEATABLE_READ;
        case fs_db::ISO_LEVEL_SERIALIZABLE:
            return store::TxIsoLevel::ISO_LEVEL_SERIALIZABLE;
        default:
            return store::TxIsoLevel::ISO_LEVEL_READ_COMMITTED;
    }
}
