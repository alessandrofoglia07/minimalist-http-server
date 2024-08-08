#include "server.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/sendfile.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

/*
 * This is a server implementation of HTTP/0.9
 * Headers: none
 * Request (only GET method supported): "GET /index.html"
 * Response: <file content>
 */
int handle_request_v0_9(const int client_fd) {
    char buffer[BUFFER_SIZE] = {0};
    if (recv(client_fd, buffer, BUFFER_SIZE, 0) < 0) {
        perror("Receive failed");
        close(client_fd);
        return -1;
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
        return -1;
    }

    // copy data from opened_fd to client_fd
    if (sendfile(client_fd, opened_fd, NULL, BUFFER_SIZE) < 0) {
        perror("Sendfile failed");
        close(opened_fd);
        close(client_fd);
        return -1;
    }

    send(client_fd, "\r\n", 2, 0);

    close(opened_fd);
    close(client_fd);

    printf("File served successfully\n");

    return 0;
}

/*
 * This is a server implementation of HTTP/1.0
 * Headers: see https://www.w3.org/Protocols/HTTP/1.0/spec.html#HeaderFields
 * Request (only GET and POST methods supported): "GET /index.html HTTP/1.0 ..."
 * Response:
 *     HTTP/1.0 200 OK
 *     ...
 *     Content-Type: text/html
 *     <file content>
 */
int handle_request_v1_0(const int client_fd) {
    char buffer[BUFFER_SIZE] = {0};
    if (recv(client_fd, buffer, BUFFER_SIZE, 0) < 0) {
        perror("Receive failed");
        close(client_fd);
        return -1;
    }

    char bufCopy[BUFFER_SIZE];
    strcpy(bufCopy, buffer);
    const char *method = strtok(bufCopy, " ");

    const char *not_found_response = "HTTP/1.0 404 Not Found\r\nContent-Length: 13\r\n\r\n404 Not Found";

    // only handle GET requests
    if (strcmp(method, "GET") != 0) {
        send(client_fd, not_found_response, strlen(not_found_response), 0);
        return 0;
    }

    // GET /file.html ...
    const char *filename = buffer + 5; // file.html ...
    *strchr(filename, ' ') = 0; // file.html

    char filepath[BUFFER_SIZE] = "www/";
    strncat(filepath, filename, sizeof(filepath) - strlen(filepath) - 1);

    const int opened_fd = open(filepath, O_RDONLY);
    if (opened_fd < 0) {
        send(client_fd, not_found_response, strlen(not_found_response), 0);
        perror("File open failed");
        close(client_fd);
        return -1;
    }

    // Send HTTP/1.0 200 OK response header
    const char *header = "HTTP/1.0 200 OK\r\nContent-Type: text/html\r\n\r\n";
    send(client_fd, header, strlen(header), 0);

    // copy data from opened_fd to client_fd
    if (sendfile(client_fd, opened_fd, NULL, BUFFER_SIZE) < 0) {
        perror("Sendfile failed");
        close(opened_fd);
        close(client_fd);
        return -1;
    }

    send(client_fd, "\r\n", 2, 0);

    close(opened_fd);
    close(client_fd);

    printf("File served successfully\n");

    return 0;
}

/*
 * This is a server implementation of HTTP/1.1
 * Headers: see https://www.w3.org/Protocols/rfc2616/rfc2616-sec14.html
 * Request: "GET /index.html HTTP/1.1 ... Connection: keep-alive"
 * Response:
 *     HTTP/1.1 200 OK
 *     ...
 *     Content-Type: text/html
 *     <file content>
 */
int handle_request_v1_1(const int client_fd) {
    char buffer[BUFFER_SIZE] = {0};
    if (recv(client_fd, buffer, BUFFER_SIZE, 0) < 0) {
        perror("Receive failed");
        close(client_fd);
        return -1;
    }

    char bufCopy[BUFFER_SIZE];
    strcpy(bufCopy, buffer);
    const char *method = strtok(bufCopy, " ");

    const char *not_found_response =
            "HTTP/1.0 404 Not Found\r\nContent-Length: 13\r\nConnection: close\r\n\r\n404 Not Found";

    // only handle GET requests
    if (strcmp(method, "GET") != 0) {
        send(client_fd, not_found_response, strlen(not_found_response), 0);
        return 0;
    }

    // GET /file.html ...
    const char *filename = buffer + 5; // file.html ...
    *strchr(filename, ' ') = 0; // file.html

    char filepath[BUFFER_SIZE] = "www/";
    strncat(filepath, filename, sizeof(filepath) - strlen(filepath) - 1);

    const int opened_fd = open(filepath, O_RDONLY);
    if (opened_fd < 0) {
        send(client_fd, not_found_response, strlen(not_found_response), 0);
        perror("File open failed");
        close(client_fd);
        return -1;
    }

    // Send HTTP/1.1 200 OK response header
    const char *header = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nTransfer-Encoding: chunked\r\n\r\n";
    send(client_fd, header, strlen(header), 0);

    // Send file data content in chunks
    off_t offset = 0;
    struct stat file_stat;
    fstat(opened_fd, &file_stat);
    size_t chunk_size = BUFFER_SIZE;

    while (file_stat.st_size > offset) {
        char chunk_header[20];

        // Determine chunk size
        if (file_stat.st_size - offset < BUFFER_SIZE) {
            chunk_size = file_stat.st_size - offset;
        }

        snprintf(chunk_header, sizeof(chunk_header), "%zx\r\n", chunk_size);
        send(client_fd, chunk_header, strlen(chunk_header), 0);

        // Send chunk data
        if (sendfile(client_fd, opened_fd, &offset, chunk_size) < 0) {
            perror("Sendfile failed");
            close(opened_fd);
            close(client_fd);
            return -1;
        }

        // End of chunk
        send(client_fd, "\r\n", 2, 0);
    }

    // End of all chunks
    send(client_fd, "0\r\n\r\n", 5, 0);

    close(opened_fd);
    close(client_fd);

    printf("File served successfully\n");

    return 0;
}

void start_server(const float httpVersion) {
    if (httpVersion != 0.9f && httpVersion != 1.0f && httpVersion != 1.1f) {
        printf("Invalid HTTP version selected. Valid options: 0.9, 1.0");
        return;
    }

    // initialize socket
    const int s = socket(AF_INET, SOCK_STREAM, 0);
    if (s < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Set socket options
    const int optval = 1;
    if (setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) < 0) {
        perror("Setsockopt failed");
        close(s);
        exit(EXIT_FAILURE);
    }

    const struct sockaddr_in addr = {
        AF_INET,
        htons(PORT) /* port 8080 -> hex(8080) -> 0x1f90 -> 0x901f*/,
        INADDR_ANY
    };

    // bound socket to local address
    if (bind(s, (struct sockaddr *) &addr, sizeof(addr)) < 0) {
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

    while (1) {
        // extract first connection in queue, creates new connected socket (not in listening state)
        // returns file descriptor of new socket
        const int client_fd = accept(s, NULL, NULL);
        if (client_fd < 0) {
            perror("Accept failed");
            continue;
        }

        if (httpVersion == 0.9f) {
            handle_request_v0_9(client_fd);
        } else if (httpVersion == 1.0f) {
            handle_request_v1_0(client_fd);
        } else if (httpVersion == 1.1f) {
            handle_request_v1_1(client_fd);
        }
    }

    close(s);
}
