#include "server.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/sendfile.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

void main() {
    // initialize socket
    const int s = socket(AF_INET, SOCK_STREAM, 0);
    const struct sockaddr addr = {
        AF_INET,
        0x901f /* port 8080 -> hex(8080) -> 0x1f90 -> 0x901f*/,
        0
    };
    // bound socket to local address
    bind(s, &addr, sizeof(addr));

    // listen for incoming requests (switch socket to listening state)
    listen(s, 10);

    // extract first connection in queue, creates new connected socket (not in listening state)
    // returns file descriptor of new socket
    const int client_fd = accept(s, 0, 0);

    char buffer[256] = {0};

    recv(client_fd, buffer, 256, 0);

    // GET /file.html ...

    const char *filename = buffer + 5; // file.html ...
    *strchr(filename, ' ') = 0; // file.html

    const int opened_fd = open(filename, O_RDONLY);

    // copy data from opened_fd to client_fd
    sendfile(client_fd, opened_fd, 0, 256);

    close(opened_fd);
    close(client_fd);
    close(s);
}