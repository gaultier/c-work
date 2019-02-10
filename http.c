#include <assert.h>
#include <sds.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sds.c>

#include "ae.h"
#include "anet.h"

static void writeToClient(aeEventLoop *loop, int fd, void *clientdata,
                          int mask) {
    time_t now_t = time(NULL);
    struct tm *now_tm = gmtime(&now_t);
    char now_s[25] = "\0";
    strftime(now_s, sizeof(now_s), "%F %T", now_tm);
    sds body = sdscatfmt(sdsempty(), "<html><bold>hello</bold><p>%s</p></html>",
                         now_s);
    sds response = sdscatfmt(
        sdsempty(),
        "HTTP/1.1 200 OK\r\nContent-Type: "
        "text/html\r\nContent-Length: %u\r\nConnection: close\r\n\r\n%s",
        sdslen(body), body);
    write(fd, response, sdslen(response));
    sdsfree(response);
    sdsfree(body);
    aeDeleteFileEvent(loop, fd, mask);
    close(fd);
}

static void readFromClient(aeEventLoop *loop, int fd, void *clientdata,
                           int mask) {
    int buffer_size = 1024;
    char *buffer = malloc(sizeof(char) * buffer_size);
    bzero(buffer, buffer_size);
    int size;
    size = read(fd, buffer, buffer_size);
    int ret = aeCreateFileEvent(loop, fd, AE_WRITABLE, writeToClient, buffer);
    assert(ret != AE_ERR);
}

static void acceptTcpHandler(aeEventLoop *loop, int fd, void *clientdata,
                             int mask) {
    int client_port, client_fd;
    char client_ip[128];
    // create client socket
    client_fd = anetTcpAccept(NULL, fd, client_ip, 128, &client_port);
    printf("Accepted %s:%d\n", client_ip, client_port);

    // set client socket non-block
    anetNonBlock(NULL, client_fd);

    // regist on message callback
    int ret;
    ret = aeCreateFileEvent(loop, client_fd, AE_READABLE, readFromClient, NULL);
    assert(ret != AE_ERR);
}

int main() {
    int ipfd;
    // create server socket
    ipfd = anetTcpServer(NULL, 8000, "0.0.0.0", 0);
    assert(ipfd != ANET_ERR);

    // create main event loop
    aeEventLoop *loop;
    loop = aeCreateEventLoop(1024);

    // regist socket connect callback
    int ret;
    ret = aeCreateFileEvent(loop, ipfd, AE_READABLE, acceptTcpHandler, NULL);
    assert(ret != AE_ERR);

    // start main loop
    aeMain(loop);

    // stop loop
    aeDeleteEventLoop(loop);

    return 0;
}
