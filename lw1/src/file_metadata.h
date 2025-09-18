// file_metadata.h — описание метаданных файла
#pragma once

#include <stdint.h>

typedef struct lcma_file_metadata_s {
    uint32_t name_length;   // длина имени файла
    uint64_t size;          // размер файла
    char *name;             // имя файла
} lcma_file_metadata_t;