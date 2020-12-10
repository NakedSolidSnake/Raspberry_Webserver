/*
 * MIT License
 *
 * Copyright (c) 2018 Lewis Van Winkle
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "http.h"
#include "client_info.h"
#include "server.h"
#include "routes.h"
#include "endpoints.h"

const char *get_directory(const char *path)
{
    const char *last_dot = strrchr(path, '.');
    if (last_dot) {
        if (strcmp(last_dot, ".css") == 0)
            return "css";
        else if (strcmp(last_dot, ".js") == 0)
            return "js";
        else if (strcmp(last_dot, ".html") == 0)
            return "page";
    }

    return "";
}

void serve_resource(struct client_info *client, const char *path) {
#define BSIZE ( 1024 * 8 )
    char buffer[BSIZE];
    int resul;
    printf("serve_resource %s %s\n", get_client_address(client), path);

    if (strlen(path) > 100) {
        HTTP_send_400(client);
        return;
    }

    if (strstr(path, "..")) {
        HTTP_send_404(client);
        return;
    }

    resul = Routes_Exec(path, buffer);
    if(!resul)
    {
        send(client->socket, buffer, strlen(buffer), 0);
    }else{
        HTTP_send_404(client);
    }
    memset(buffer, 0, sizeof buffer);
    
    drop_client(client);
}


int main() {

    Routes_Add("/", Endpoint_Index);
    Routes_Add("/style.css", Endpoint_Index);
    Routes_Add("/script.js", Endpoint_Index);
    Routes_Add("/led_on", Endpoint_LEDOn);
    Routes_Add("/led_off", Endpoint_LEDOff);

    SOCKET server = create_socket(0, "8989");
    server_execute(server);

    printf("\nClosing socket...\n");
    CLOSESOCKET(server);

    printf("Finished.\n");
    return 0;
}

