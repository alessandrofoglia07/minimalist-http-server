#ifndef SERVER_H
#define SERVER_H

#define PORT 8080
#define BUFFER_SIZE 256

int handle_client_v0_9(const int client_fd);

int handle_client_v1_0(const int client_fd);

void start_server(const float httpVersion);

#endif //SERVER_H
