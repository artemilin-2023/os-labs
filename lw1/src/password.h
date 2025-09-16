#pragma once

#include <stddef.h>

typedef struct lcma_password_s {
	char *data;        // нуль-терминированная строка пароля
	size_t length;     // длина без нуль-терминатора
} lcma_password_t;

// Инициализация структуры пароля
int password_init(lcma_password_t *pw, const char *input);

// Затирание и освобождение
void password_destroy(lcma_password_t *pw);


