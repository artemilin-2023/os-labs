#pragma once

#include <stddef.h>

// Архивация: создание архива из файла/директории
int archive_create(const char *input_path, const char *archive_path, const char *password);

// Добавление одного файла в уже создаваемый архив — опционально (заглушка)
int archive_add_file(const char *archive_path, const char *file_path);