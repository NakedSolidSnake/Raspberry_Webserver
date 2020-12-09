#ifndef SERVER_H_
#define SERVER_H_

#include "definitions.h"
#include "client_info.h"

SOCKET create_socket(const char* host, const char *port);
struct client_info *get_client(SOCKET s);
void drop_client(struct client_info *client);
const char *get_client_address(struct client_info *ci);
fd_set wait_on_clients(SOCKET server);
int server_execute(SOCKET server);


#endif /* SERVER_H_ */
