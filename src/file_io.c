#include "file_io.h"
#include <stdio.h>
#include <stdlib.h>

u8 *ReadFileData(i32 *bytes, const char *filename)
{
    FILE *file = fopen(filename, "rb");
    if (!file)
    {
        *bytes = 0;
        return NULL;
    }

    fseek(file, 0, SEEK_END);
    *bytes = ftell(file);

    rewind(file);
    u8 *data = malloc(*bytes);
    fread(data, 1, *bytes, file);

    fclose(file);
    return data;
}
