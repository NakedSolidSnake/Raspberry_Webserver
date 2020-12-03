#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdbool.h>
#include <limits.h>
#include <pthread.h>

#define SERVERPORT      8989
#define BUFSIZE         4096
#define SOCKETERROR     (-1)
#define SERVER_BACKLOG  100
// #define THREAD_POLL_SIZE    20
#define THREAD_POLL_SIZE    1


typedef struct
{
    char *content;
    long size;
} File;

typedef struct 
{
    File css;
    File javascript;
    File html;
} Page;

typedef struct sockaddr_in SA_IN;
typedef struct sockaddr    SA;

pthread_t thread_pool[THREAD_POLL_SIZE];
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

void *handle_connection(void *args);
int check(int exp, const char *msg);

void *thread_function(void *args);

int loadCSS(File *css);
int loadJavascript(File *javascript);
int loadHTML(File *html);
char *Parse_getService(char *buffer);

int buildPage(Page *page);
int destroyPage(Page *page);

long getFileSize(FILE *file);

int main(int argc, char const *argv[])
{
    int server_socket, client_socket, addr_size;
    SA_IN server_addr, client_addr;

    for (int i = 0; i < THREAD_POLL_SIZE; i++)
    {
        pthread_create(&thread_pool[i], NULL, thread_function, NULL);
    }
    

    check((server_socket = socket(AF_INET, SOCK_STREAM, 0)), "Failed to create socket");

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(SERVERPORT);

    int yes = 1;
    if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, (void *)&yes, sizeof(yes)) < 0)
    {
        fprintf(stderr, "setsockopt() failed.\n");
    }

    check(bind(server_socket, (SA*)&server_addr, sizeof(server_addr)), "Bind Failed!");

    check(listen(server_socket, SERVER_BACKLOG), "Listen Failed!");

    while(true)
    {
        printf("Waiting for connections...\n");

        addr_size = sizeof(SA_IN);
        
        check(client_socket = accept(server_socket, (SA*)&client_addr, (socklen_t *)&addr_size), "Accept Failed!");

        printf("Connected!\n");

        // pthread_t t;
        int *pclient = malloc(sizeof(int));
        *pclient = client_socket;
        pthread_mutex_lock(&mutex);
        enqueue(pclient);
        pthread_cond_signal(&cond);
        pthread_mutex_unlock(&mutex);
        // pthread_create(&t, NULL, handle_connection, pclient);
    }
    return 0;
}

void *thread_function(void *args)
{
    while(true){
        int *pclient;
        pthread_mutex_lock(&mutex);
        if((pclient = dequeue()) == NULL){
            pthread_cond_wait(&cond, &mutex);
            pclient = dequeue();
        }        
        pthread_mutex_unlock(&mutex);
        if(pclient != NULL){
            handle_connection(pclient);
        }
    }
}

int check(int exp, const char *msg)
{
    if(exp == SOCKETERROR){
        perror(msg);
        exit(1);
    }

    return exp;
}

void *handle_connection(void *args)
{
    char buffer[BUFSIZE];
    size_t bytes_read;

    int client_socket = *(int *)args;

    free(args);

    int msgsize = 0;
    char actualpath[_PC_PATH_MAX + 1];

    while((bytes_read = recv(client_socket, buffer+msgsize, sizeof(buffer) - msgsize - 1, 0)) > 0){
        msgsize += bytes_read;
        if(msgsize > BUFSIZE - 1 || buffer[msgsize - 1] == '\n')
            break;
    }    
    
    
    printf("REQUEST: %s\n", buffer);    

    //verify which endpoint was received    

    char *pService = Parse_getService(buffer);
        
    if(pService[0] == '/')
    {

        Page page;
        buildPage(&page);

        memset(buffer, 0, BUFSIZE);
        sprintf(buffer, "HTTP/1.1 200 OK\r\n");
        send(client_socket, buffer, strlen(buffer), 0);

        sprintf(buffer, "Connection: close\r\n");
        send(client_socket, buffer, strlen(buffer), 0);

        sprintf(buffer, "Content-Length: %ld\r\n", page.html.size);
        send(client_socket, buffer, strlen(buffer), 0);

        sprintf(buffer, "Content-Type: %s\r\n", "text/html");
        send(client_socket, buffer, strlen(buffer), 0);

        sprintf(buffer, "\r\n");
        send(client_socket, buffer, strlen(buffer), 0);

        send(client_socket, page.html.content, strlen(page.html.content), 0);

        destroyPage(&page);

    }else{
        const char *c404 = "HTTP/1.1 404 Not Found\r\n"
        "Connection: close\r\n"
        "Content-Length: 9\r\n\r\nNot Found";
        send(client_socket, c404, strlen(c404), 0);
    }

    close(client_socket);
    // fclose(fp);
    printf("Closing connection\n");
    return NULL;
}

int loadCSS(File *css)
{
    if(css)
    {
        FILE *file = fopen("css/style.css", "r");
        if(file)
        {
            css->size = getFileSize(file);
            css->content = (char *)malloc(sizeof(char) * css->size + 1);
            if(css->content)
            {
                size_t read_total = fread(css->content, 1, css->size + 1, file);                                
            }

            fclose(file);
        }
    }
}

int loadJavascript(File *javascript)
{
    if(javascript)
    {
        FILE *file = fopen("js/script.js", "r");
        if(file)
        {
            javascript->size = getFileSize(file);
            javascript->content = (char *)malloc(sizeof(char) * javascript->size + 1);
            if(javascript->content)
            {
                size_t read_total = fread(javascript->content, 1, javascript->size + 1, file);                                
            }

            fclose(file);
        }
    }
}

int loadHTML(File *html)
{
    if(html)
    {
        FILE *file = fopen("page/index.html", "r");
        if(file)
        {
            html->size = getFileSize(file);
            html->content = (char *)malloc(sizeof(char) * html->size + 1);
            if(html->content)
            {
                size_t read_total = fread(html->content, 1, html->size + 1, file);                                
            }

            fclose(file);
        }
    }
}

long getFileSize(FILE *file)
{
    long size = 0;
    if(file)
    {
        fseek(file, 0, SEEK_END);
        size = ftell(file);
        rewind(file);
    }
    return size;
}

char *Parse_getService(char *buffer)
{   
    char *pch = NULL;
    char *end = NULL;
    char *pBuffer = buffer;
    pch=strchr(buffer, '/');

    for(int i = 0; ; i++)
    {
        if(!pch)
            return NULL;

        if(*(pch + i) == '?' || *(pch + i) == ' ')
        {
            end = (pch + i);
            break;
        }
    }

    return strndup(pch, end - pch);
}

int buildPage(Page *page)
{
    loadCSS(&page->css);
    loadJavascript(&page->javascript);
    loadHTML(&page->html);

    return 0;
}

int destroyPage(Page *page)
{
    free(page->css.content);
    free(page->css.content);
    free(page->css.content);
}