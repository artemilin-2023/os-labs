#include "archiver.h"
#include "archive_format.h"
#include "fileutils.h"
#include <stdio.h>
#include "file_metadata.h"

static int process_file(const char *file_path, void *args);

typedef struct {
    FILE *out;
    const char *input_path;
} file_process_dto;

// input: C:\Users\LightChimera\Desktop
//
// files: C:\Users\LightChimera\Desktop\text.txt
// files: C:\Users\LightChimera\Desktop\bebra\text2.txt
//
// rel: text.txt
// rel: bebra\text2.txt

int archive_create(const char *input_path, const char *archive_path, const char *password) {
	(void)password; // TODO: использовать при шифровании

	FILE *out = fopen(archive_path, "wb");
	if (!out) return -1;

	lcma_archive_header_t hdr = {0};
	hdr.magic = LCMA_MAGIC;
	hdr.version = LCMA_VERSION;
    hdr.file_count = get_file_count(input_path);

	if (fwrite(&hdr, sizeof(hdr), 1, out) != 1) {
		fclose(out);
		return -2;
	}

	file_process_dto dto = {
		.out = out,
		.input_path = input_path
	};
    walk_directory(input_path, process_file, &dto);

	fclose(out);
	return 0;
}

static int process_file(const char *file_path, void *args) {
	file_process_dto *dto = (file_process_dto *)args;
    FILE *out = dto->out;
	char *input_path = dto->input_path;

	FILE *input = fopen(file_path, "r");

	char *relative_path = get_relative_path(input_path, file_path);
    
	lcma_file_metadata_t metadata = {0};
    metadata.name_length = strlen(relative_path);
    metadata.name = relative_path;
    metadata.size = get_file_size(input);

    return 0;
}

int archive_add_file(const char *archive_path, const char *file_path) {
	(void)archive_path;
	(void)file_path;
	return 0;
}