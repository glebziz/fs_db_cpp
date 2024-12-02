#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <fs_db/connection.h>
#include <fs_db/transaction.h>
#include <fs_db/store.h>
#include <fs_db/write_file.h>
#include <fs_db/read_file.h>

#define KEY "someCKey"
#define CONTENT "someCContent"
#define CONTENT2 "someCContent2"

int main() {
    fs_db_conn *conn = fs_db_connect("localhost:8888");

    fs_db_wf *wf;
    fs_db_err err = fs_db_set_file((fs_db_store*)conn, KEY, &wf);
    if (err) {
        printf("%s\n", fs_db_err_msg(err));
        return 1;
    }

    err = fs_db_wf_write(wf, CONTENT, strlen(CONTENT));
    if (err) {
        printf("%s\n", fs_db_err_msg(err));
        return 1;
    }

    err = fs_db_wf_close(&wf);
    if (err) {
        printf("%s\n", fs_db_err_msg(err));
        return 1;
    }

    fs_db_rf *rf;
    err = fs_db_get_file((fs_db_store*)conn, KEY, &rf);
    if (err) {
        printf("%s\n", fs_db_err_msg(err));
        return 1;
    }

    size_t read_size;
    char data[10];
    while (!((err = fs_db_rf_read(rf, data, sizeof(data), &read_size))) && read_size) {
        printf("%s", data);
        memset(data, 0, sizeof(data));
    }
    if (err) {
        printf("%s\n", fs_db_err_msg(err));
        return 1;
    }

    err = fs_db_rf_close(&rf);
    if (err) {
        printf("%s\n", fs_db_err_msg(err));
        return 1;
    }
    printf("\n");

    err = fs_db_delete((fs_db_store*)conn, KEY);
    if (err) {
        printf("%s\n", fs_db_err_msg(err));
        return 1;
    }

    fs_db_tx *tx;
    err = fs_db_begin(conn, ISO_LEVEL_DEFAULT, &tx);
    if (err) {
        printf("%s\n", fs_db_err_msg(err));
        return 1;
    }

    err = fs_db_set((fs_db_store*)tx, KEY, CONTENT2, strlen(CONTENT2));
    if (err) {
        printf("%s\n", fs_db_err_msg(err));
        return 1;
    }

    char *content;
    size_t size;
    err = fs_db_get((fs_db_store*)tx, KEY, &content, &size);
    if (err) {
        printf("%s\n", fs_db_err_msg(err));
        return 1;
    }

    printf("%s\n", content);
    free(content);

    err = fs_db_tx_rollback(&tx);
    if (err) {
        printf("%s\n", fs_db_err_msg(err));
        return 1;
    }

    err = fs_db_get_file((fs_db_store*)conn, KEY, &rf);
    if (err != NOT_FOUND_ERR) {
        printf("%s\n", fs_db_err_msg(err));
        return 1;
    }

    fs_db_conn_destroy(&conn);

    return 0;
}
