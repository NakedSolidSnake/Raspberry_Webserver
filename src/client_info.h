#ifndef CLIENT_INFO_H_
#define CLIENT_INFO_H_

#include "definitions.h"

#define MAX_REQUEST_SIZE 2047

struct client_info {
    socklen_t address_length;
    struct sockaddr_storage address;
    SOCKET socket;
    char request[MAX_REQUEST_SIZE + 1];
    int received;
    struct client_info *next;
};

#endif /* CLIENT_INFO_H_ */
