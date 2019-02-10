#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "ae.h"
#include "anet.h"

static void writeToClient(aeEventLoop *loop, int fd, void *clientdata,
                          int mask) {
    char body[] = "<html><bold>hello</bold></html>";
    char *buffer = calloc(1, 1000);
    snprintf(buffer, 1000,
             "HTTP/1.1 200 OK\r\nContent-Type: "
             "text/html\r\nContent-Length: %zu\r\nConnection: close\r\n\r\n%s",
             strlen(body), body);
    write(fd, buffer, strlen(buffer));
    free(buffer);
    aeDeleteFileEvent(loop, fd, mask);
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
