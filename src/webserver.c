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
#include <led.h>
#include "server.h"

static eState_t eState = eStateLow;

static LED_t led = {
        .gpio.pin = 0,
        .gpio.eMode = eModeOutput
    };

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
#define BSIZE 1024
    char buffer[BSIZE];
    printf("serve_resource %s %s\n", get_client_address(client), path);

    if (strcmp(path, "/") == 0)
        path = "/index.html";

    if (strncmp(path, "/led_on", 7) == 0)
    {
        eState = eStateHigh;
        LED_set(&led, eState);

        sprintf(buffer, "HTTP/1.1 200 OK\r\n");
        send(client->socket, buffer, strlen(buffer), 0);

        sprintf(buffer, "Connection: close\r\n");
        send(client->socket, buffer, strlen(buffer), 0);

        sprintf(buffer, "Content-Length: %u\r\n", 1);
        send(client->socket, buffer, strlen(buffer), 0);

        sprintf(buffer, "Content-Type: %s\r\n", "text/plain");
        send(client->socket, buffer, strlen(buffer), 0);

        sprintf(buffer, "\r\n");
        send(client->socket, buffer, strlen(buffer), 0);

        sprintf(buffer, "%d", eState);
        send(client->socket, buffer, strlen(buffer), 0);

        drop_client(client);
        return;
    }
        

    if (strncmp(path, "/led_off", 8) == 0)
    {
        eState = eStateLow;
        LED_set(&led, eState);

        sprintf(buffer, "HTTP/1.1 200 OK\r\n");
        send(client->socket, buffer, strlen(buffer), 0);

        sprintf(buffer, "Connection: close\r\n");
        send(client->socket, buffer, strlen(buffer), 0);

        sprintf(buffer, "Content-Length: %u\r\n", 1);
        send(client->socket, buffer, strlen(buffer), 0);

        sprintf(buffer, "Content-Type: %s\r\n", "text/plain");
        send(client->socket, buffer, strlen(buffer), 0);

        sprintf(buffer, "\r\n");
        send(client->socket, buffer, strlen(buffer), 0);

        sprintf(buffer, "%d", eState);
        send(client->socket, buffer, strlen(buffer), 0);

        drop_client(client);
        return;
    }

    if (strlen(path) > 100) {
        HTTP_send_400(client);
        return;
    }

    if (strstr(path, "..")) {
        HTTP_send_404(client);
        return;
    }

    char full_path[128];
    sprintf(full_path, "%s%s", get_directory(path), path);

    printf("full_path = %s.\n", full_path);
    FILE *fp = fopen(full_path, "rb");

    if (!fp) {
        HTTP_send_404(client);
        return;
    }

    fseek(fp, 0L, SEEK_END);
    size_t cl = ftell(fp);
    rewind(fp);

    const char *ct = HTTP_get_content_type(full_path);

    sprintf(buffer, "HTTP/1.1 200 OK\r\n");
    send(client->socket, buffer, strlen(buffer), 0);

    sprintf(buffer, "Connection: close\r\n");
    send(client->socket, buffer, strlen(buffer), 0);

    sprintf(buffer, "Content-Length: %u\r\n", cl);
    send(client->socket, buffer, strlen(buffer), 0);

    sprintf(buffer, "Content-Type: %s\r\n", ct);
    send(client->socket, buffer, strlen(buffer), 0);

    sprintf(buffer, "\r\n");
    send(client->socket, buffer, strlen(buffer), 0);

    int r = fread(buffer, 1, BSIZE, fp);
    while (r) {
        send(client->socket, buffer, r, 0);
        r = fread(buffer, 1, BSIZE, fp);
    }    

    fclose(fp);
    drop_client(client);
}


int main() {

    if(LED_init(&led))
        return EXIT_FAILURE;

    SOCKET server = create_socket(0, "8989");
    server_execute(server);

    printf("\nClosing socket...\n");
    CLOSESOCKET(server);

    printf("Finished.\n");
    return 0;
}

