#ifndef HTTP_H_
#define HTTP_H_

#include "client_info.h"

const char *HTTP_get_content_type(const char* path);
void HTTP_send_400(struct client_info *client);
void HTTP_send_404(struct client_info *client);

#endif /* HTTP_H_ */
