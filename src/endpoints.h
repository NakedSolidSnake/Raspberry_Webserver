#ifndef ENDPOINTS_H_
#define ENDPOINTS_H_

int Endpoint_Index(const char *path, char *buffer, void *data);
int Endpoint_LEDOn(const char *path, char *buffer, void *data);
int Endpoint_LEDOff(const char *path, char *buffer, void *data);

#endif /* ENDPOINTS_H_ */
