#include "parser.h"

#include <stdio.h>
#include <stdlib.h>
#include <core/AGmemory.h>

char* read_file(const char* file_path, u64* out_length) {
    FILE* file = fopen(file_path, "r");
    if (!file) {
        return NULL;
    }

    fseek(file, 0, SEEK_END);
    long length = ftell(file);
    fseek(file, 0, SEEK_SET);

    if (out_length) {
        *out_length = (u64)length;
    }

    char* buffer = (char*)AGallocate(length + 1, MEMORY_TAG_STRING);
    if (!buffer) {
        fclose(file);
        return NULL;
    }

    fread(buffer, 1, length, file);
    buffer[length] = '\0';

    fclose(file);
    return buffer;
}