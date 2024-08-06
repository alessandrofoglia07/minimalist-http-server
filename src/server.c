#include "server.h"
#include <sys/types.h>
#include <sys/socket.h>

void main() {
    int s = socket(AF_INET, SOCK_STREAM, 0);
}