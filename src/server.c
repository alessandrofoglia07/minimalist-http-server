#include "server.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/sendfile.h>
#include <string.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define PORT 8080
#define BUFFER_SIZE 256

// Current HTTP version: 0.9 (when testing, specify HTTP/0.9 version)
int main() {
    // initialize socket
    const int s = socket(AF_INET, SOCK_STREAM, 0);
    if (s < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    const struct sockaddr_in addr = {
        AF_INET,
        htons(PORT) /* port 8080 -> hex(8080) -> 0x1f90 -> 0x901f*/,
        INADDR_ANY
    };

    // bound socket to local address
    if (bind(s, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("Bind failed");
        close(s);
        exit(EXIT_FAILURE);
    }

    // listen for incoming requests (switch socket to listening state)
    if (listen(s, 10) < 0) {
        perror("Listen failed");
        close(s);
        exit(EXIT_FAILURE);
    }

    printf("Server is listening on port %d\n", PORT);

    // extract first connection in queue, creates new connected socket (not in listening state)
    // returns file descriptor of new socket
    const int client_fd = accept(s, NULL, NULL);
    if (client_fd < 0) {
        perror("Accept failed");
        close(s);
        exit(EXIT_FAILURE);
    }

    char buffer[BUFFER_SIZE] = {0};
    if (recv(client_fd, buffer, BUFFER_SIZE, 0) < 0) {
        perror("Receive failed");
        close(client_fd);
        close(s);
        exit(EXIT_FAILURE);
    }

    // GET /file.html ...
    const char *filename = buffer + 5; // file.html ...
    *strchr(filename, ' ') = 0; // file.html

    char filepath[BUFFER_SIZE] = "www/";
    strncat(filepath, filename, sizeof(filepath) - strlen(filepath) - 1);

    const int opened_fd = open(filepath, O_RDONLY);
    if (opened_fd < 0) {
        perror("File open failed");
        close(client_fd);
        close(s);
        exit(EXIT_FAILURE);
    }

    // copy data from opened_fd to client_fd
    if (sendfile(client_fd, opened_fd, NULL, BUFFER_SIZE) < 0) {
        perror("Sendfile failed");
        close(opened_fd);
        close(client_fd);
        close(s);
        exit(EXIT_FAILURE);
    }

    close(opened_fd);
    close(client_fd);
    close(s);

    printf("File served successfully\n");

    return 0;
}