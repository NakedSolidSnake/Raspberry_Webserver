#include "file.h"

int FILE_GetDataFromFile(const char *file, char *buffer)
{
    long size;
    int resul = -1;
    FILE *fp = fopen(file, "rb");
    if(!fp)
        return resul;

    size = FILE_GetSize(fp);

    size_t data_read = fread(buffer, 1, size, fp);
    if(data_read == size)
    {
        resul = 0;
    }

    fclose(fp);

    return resul;
}

long FILE_GetSize(FILE *file)
{
    long file_size = -1;
    if(file)
    {
        fseek(file, 0L, SEEK_END);
        file_size = ftell(file);
        rewind(file);
    }

    return file_size;
}