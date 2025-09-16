// archive_format.h — описание формата архива и заголовков
#pragma once

#include <stdint.h>

#define LCMA_MAGIC 0x4C434D41u /* 'LCMA' */
#define LCMA_VERSION 1u

// Флаги архива
enum lcma_archive_flags {
	LCMA_FLAG_PASSWORD = 1u << 0,
	LCMA_FLAG_COMPRESSED = 1u << 1
};

// Заголовок архива
typedef struct lcma_archive_header_s {
	uint32_t magic;         // сигнатура
	uint16_t version;      	// версия формата
	uint16_t flags;         // флаги
	uint32_t file_count;    // количество файлов внутри
	uint32_t reserved;      // выравнивание/резерв
} lcma_archive_header_t;

// Параметры шифрования (не факт что сделаю)
typedef struct lcma_crypto_params_s {
	uint8_t salt[16];
	uint8_t iv[16];
	uint32_t kdf_iterations; // для PBKDF2/scrypt/argon2 — пока заглушка
} lcma_crypto_params_t;


