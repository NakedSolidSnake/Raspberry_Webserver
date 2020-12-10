#ifndef FILE_H_
#define FILE_H_

#include <stdio.h>

int FILE_GetDataFromFile(const char *filename, char *buffer);
long FILE_GetSize(FILE *file);

#endif /* FILE_H_ */
