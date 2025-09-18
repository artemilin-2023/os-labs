#include "archiver.h"
#include "archive_format.h"
#include "fileutils.h"
#include <stdio.h>
#include "file_metadata.h"
#include "constants.h"
#include "streamutils.h"

static int fill_archive_header(const char *input_path, FILE *out);
static int fill_file_metadata(const char *file_path, void *args);
static int fill_payload(const char *input_path, FILE *out);
static int fill_payload_file_process(const char *file_path, void *args);

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
	if (!out) return FAIL_CANT_OPEN_FILE;

    int fillHeaderResult = fill_archive_header(input_path, out);
	if (fillHeaderResult != SUCCESS)
	{
		fclose(out);
		return fillHeaderResult;
	}

	int fill_payload_result = fill_payload(input_path, out);
    fclose(out);

	return fill_payload_result;
}

static int fill_archive_header(const char *input_path, FILE *out)
{
    lcma_archive_header_t hdr = {0};
    hdr.magic = LCMA_MAGIC;
    hdr.version = LCMA_VERSION;
    hdr.file_count = get_file_count(input_path);

    if (fwrite(&hdr, sizeof(hdr), 1, out) != TRUE)
    {
        fclose(out);
        return FAIL_CANT_WRITE_DATA;
    }

    file_process_dto dto = {
        .out = out,
        .input_path = input_path
	};
    return walk_directory(input_path, fill_file_metadata, &dto);
}

static int fill_file_metadata(const char *file_path, void *args) {
	file_process_dto *dto = (file_process_dto *)args;
    FILE *out = dto->out;
    const char *input_path = dto->input_path;

	FILE *input = fopen(file_path, "rb");

    const char *relative_path = get_relative_path(input_path, file_path);
    
	lcma_file_metadata_t metadata = {0};
    metadata.name_length = strlen(relative_path);
    metadata.name = relative_path;
    metadata.size = get_file_size(input);

	if (fwrite(&metadata, sizeof(metadata), 1, out) != TRUE)
	{
		fclose(input);
		return FAIL_CANT_WRITE_DATA;
	}

	fclose(input);
    return SUCCESS;
}

static int fill_payload(const char *input_path, FILE *out)
{
	file_process_dto dto = {
        .out = out,
        .input_path = input_path
	};
	return walk_directory(input_path, fill_payload_file_process, &dto);
}

static int fill_payload_file_process(const char *file_path, void *args)
{
	file_process_dto *dto = (file_process_dto *)args;
    FILE *out = dto->out;
    const char *input_path = dto->input_path;

	FILE *input = fopen(file_path, "rb");
	stream_copy(input, out);

	fclose(input);
	return SUCCESS;
}

int archive_add_file(const char *archive_path, const char *file_path) {
	(void)archive_path;
	(void)file_path;
	return 0;
}