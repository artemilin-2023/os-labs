#pragma once

#include "password.h"

int archive_extract(const char *archive_path, const char *output_dir, const lcma_password_t *password);
