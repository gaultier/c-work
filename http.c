#include <assert.h>
#include <mysql/mysql.h>
#include <sds.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sds.c>

#include "ae.h"
#include "anet.h"

#define CHECK(fn, error)                                                   \
    do {                                                                   \
        if (!(fn)) {                                                       \
            fprintf(stderr, "%s:%d:%s: %s (function was: %s)\n", __FILE__, \
                    __LINE__, __FUNCTION__, error, #fn);                   \
            exit(1);                                                       \
        }                                                                  \
    } while (0)

struct in_data {
    char *buffer;
    MYSQL *mysql;
};

static sds csv_rows(void *clientdata) {
    struct in_data *d = clientdata;
    const char query[] =
        "select external_reference from tab_merchant order by id desc limit 5";
    MYSQL_STMT *stmt = mysql_stmt_init(d->mysql);
    CHECK((mysql_stmt_prepare(stmt, query, strlen(query)) == 0),
          mysql_stmt_error(stmt));

    char external_reference[256] = "\0";
    const size_t buffer_length = 256;
    MYSQL_BIND bind_out[] = {{.buffer_type = MYSQL_TYPE_STRING,
                              .buffer = external_reference,
                              .buffer_length = buffer_length}};
    CHECK((mysql_stmt_bind_result(stmt, bind_out) == 0),
          mysql_stmt_error(stmt));
    CHECK((mysql_stmt_execute(stmt) == 0), mysql_stmt_error(stmt));
    CHECK((mysql_stmt_store_result(stmt) == 0), mysql_stmt_error(stmt));

    sds csv = sdsempty();
    int fetch_result = 0;
    while ((fetch_result = mysql_stmt_fetch(stmt)) == 0) {
        const char *s = bind_out[0].buffer;
        csv = sdscat(csv, s);
        csv = sdscat(csv, "\n");
    }
    if (fetch_result != MYSQL_NO_DATA) {
        fprintf(stderr, "Error fetching rows: %d %d %s\n", fetch_result,
                mysql_stmt_errno(stmt), mysql_stmt_error(stmt));
        exit(1);
    }

    return csv;
}

static void writeToClient(aeEventLoop *loop, int fd, void *clientdata,
                          int mask) {
    sds rows = csv_rows(clientdata);
    sds response = sdscatfmt(
        sdsempty(),
        "HTTP/1.1 200 OK\r\nContent-Type: "
        "text/csv\r\nContent-Length: %u\r\nContent-Disposition: attachment; "
        "filename=\"test.csv\"\r\nConnection: close\r\n\r\n%s",
        sdslen(rows), rows);
    write(fd, response, sdslen(response));
    sdsfree(response);
    sdsfree(rows);
    aeDeleteFileEvent(loop, fd, mask);
    close(fd);
}

static void readFromClient(aeEventLoop *loop, int fd, void *clientdata,
                           int mask) {
    (void)mask;
    int buffer_size = 1024;
    char *buffer = malloc(sizeof(char) * buffer_size);
    bzero(buffer, buffer_size);
    int size;
    size = read(fd, buffer, buffer_size);
    ((struct in_data *)clientdata)->buffer = buffer;
    int ret =
        aeCreateFileEvent(loop, fd, AE_WRITABLE, writeToClient, clientdata);
    assert(ret != AE_ERR);
    free(buffer);
}

static void acceptTcpHandler(aeEventLoop *loop, int fd, void *clientdata,
                             int mask) {
    (void)mask;
    int client_port, client_fd;
    char client_ip[128];
    // create client socket
    client_fd = anetTcpAccept(NULL, fd, client_ip, 128, &client_port);
    printf("Accepted %s:%d\n", client_ip, client_port);

    // set client socket non-block
    anetNonBlock(NULL, client_fd);

    // regist on message callback
    int ret;
    ret = aeCreateFileEvent(loop, client_fd, AE_READABLE, readFromClient,
                            clientdata);
    assert(ret != AE_ERR);
}

int main() {
    MYSQL mysql;

    mysql_init(&mysql);
    mysql_options(&mysql, MYSQL_READ_DEFAULT_GROUP, "your_prog_name");
    if (!mysql_real_connect(&mysql, "localhost", "girolink", "girolink",
                            "girolink", 0, NULL, 0)) {
        fprintf(stderr, "Failed to connect to database: Error: %s\n",
                mysql_error(&mysql));
    }

    int ipfd;
    // create server socket
    ipfd = anetTcpServer(NULL, 8000, "0.0.0.0", 0);
    assert(ipfd != ANET_ERR);

    // create main event loop
    aeEventLoop *loop;
    loop = aeCreateEventLoop(1024);

    // regist socket connect callback
    int ret;
    struct in_data d = {.mysql = &mysql};
    ret = aeCreateFileEvent(loop, ipfd, AE_READABLE, acceptTcpHandler, &d);
    assert(ret != AE_ERR);

    // start main loop
    aeMain(loop);

    // stop loop
    aeDeleteEventLoop(loop);

    return 0;
}
