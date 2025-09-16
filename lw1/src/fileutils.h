#pragma once

#include <stddef.h>

int ensure_directory_exists(const char *path);

typedef int (*file_visit_cb)(const char *path, void *user_data);
int walk_directory(const char *root, file_visit_cb cb, void *user_data);

int get_file_count(const char *root);

uint64_t get_file_size(FILE *file);

char *get_relative_path(const char *root, const char *path);