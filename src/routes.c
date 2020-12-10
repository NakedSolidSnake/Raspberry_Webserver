#include "routes.h"
#include <string.h>

typedef struct 
{
    Routes routes[MAX_ROUTES_AMOUNT];
    unsigned char amount;
} _Routes;

static _Routes _routes = {
    .routes = {0},
    .amount = 0
};


int Routes_Add(const char *endpoint, const Exec exec)
{
    int resul = -1;
    if(_routes.amount < MAX_ROUTES_AMOUNT)
    {
        strncpy(_routes.routes[_routes.amount].endpoint, endpoint, MAX_ENDPOINT_LEN + 1);
        _routes.routes[_routes.amount].exec = exec;
        _routes.amount++;
        resul = 0;
    }
    return resul;
}

int Routes_Exec(const char *endpoint, const char *buffer)
{
    int resul = -1;
    for(int i = 0; i < _routes.amount; i++)
    {
        if(!strcmp(endpoint, _routes.routes[i].endpoint))
        {
            if(_routes.routes[i].exec)
            {
                _routes.routes[i].exec(endpoint, buffer, NULL);                
                resul = 0;                
            }

            break;            
        }
    }

    return resul;
}