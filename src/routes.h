#ifndef ROUTES_H_
#define ROUTES_H_

#define MAX_ENDPOINT_LEN    30
#define MAX_ROUTES_AMOUNT   10

typedef void (*Exec)(const char *path, char *buffer, void *data);


typedef struct 
{
    char endpoint[MAX_ENDPOINT_LEN + 1];
    Exec exec;
} Routes;

int Routes_Add(const char *endpoint, const Exec exec);
int Routes_Exec(const char *endpoint, const char *buffer);

#endif /* ROUTES_H_ */
