#include "endpoints.h"
#include <stdio.h>
#include <stdlib.h>
#include <led.h>

static eState_t eState = eStateLow;

static LED_t led = {
        .gpio.pin = 0,
        .gpio.eMode = eModeOutput
    };

static int initLed(void)
{
    static int init = 0;
    if(init)
    {
        init = 1;
        return EXIT_SUCCESS;
    }

    if(LED_init(&led))
        return EXIT_FAILURE;
}

int Endpoint_Index(const char *path, char *buffer, void *data)
{
    int resul;    
    char data_file[BUFSIZ];
    const char *answer = 
    {
        "HTTP/1.1 200 OK\r\n"
        "Connection: close\r\n"
        "Content-Length: %u\r\n"
        "Content-Type: %s\r\n"
        "\r\n"
        "%s"
    };

    char *index = "/index.html";
    char full_path[128];
    if(!strcmp(path, "/"))
        sprintf(full_path, "%s%s", get_directory(index), index);
    else
        sprintf(full_path, "%s%s", get_directory(path), path);


    resul = FILE_GetDataFromFile(full_path, data_file);
    if(resul == -1)
        return resul;   

    printf("full_path = %s.\n", full_path);   

    const char *ct = HTTP_get_content_type(full_path);

    snprintf(buffer, BUFSIZ, answer, strlen(data_file), ct, data_file);

    return 0;    
}

int Endpoint_LEDOn(const char *path, char *buffer, void *data)
{
    const char *answer = 
    {
        "HTTP/1.1 200 OK\r\n"
        "Connection: close\r\n"
        "Content-Length: %u\r\n"
        "Content-Type: %s\r\n"
        "\r\n"
        "%d"
    };
    initLed();
    eState = eStateHigh;
    LED_set(&led, eState);
    sprintf(buffer, answer, 1, "text/plain", eState);
    return 0;
}

int Endpoint_LEDOff(const char *path, char *buffer, void *data)
{    
    const char *answer = 
    {
        "HTTP/1.1 200 OK\r\n"
        "Connection: close\r\n"
        "Content-Length: %u\r\n"
        "Content-Type: %s\r\n"
        "\r\n"
        "%d"
    };
    initLed();
    eState = eStateLow;
    LED_set(&led, eState);
    sprintf(buffer, answer, 1, "text/plain", eState);
    return 0;
}