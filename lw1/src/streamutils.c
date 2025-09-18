#include "streamutils.h"

void stream_copy(FILE *in, FILE *out)
{
    char buffer[4096];
    size_t bytes_read;
    while ((bytes_read = fread(buffer, 1, sizeof(buffer), in)) > 0) {
        fwrite(buffer, 1, bytes_read, out);
    }
}