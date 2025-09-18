#pragma once

#include <stddef.h>
#include "password.h"

// Архивация: создание архива из файла/директории
int archive_create(const char *input_path, const char *archive_path, const lcma_password_t *password);