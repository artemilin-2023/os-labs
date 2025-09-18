#include "unarchiver.h"
#include "archive_format.h"
#include "file_metadata.h"
#include <stdio.h>
#include "constants.h"
#include "fileutils.h"

int archive_extract(const char *archive_path, const char *output_dir, const char *password) {
	(void)password;

	FILE *in = fopen(archive_path, "rb");
	if (!in) return FAIL_CANT_OPEN_FILE;

	lcma_archive_header_t hdr = {0};
	if (fread(&hdr, sizeof(hdr), 1, in) != TRUE) {
		fclose(in);
		return FAIL_CANT_READ_DATA;
	}

	if (hdr.magic != LCMA_MAGIC) {
		fclose(in);
		return FAIL_INVALID_MAGIC_NUMBER;
	}

	ensure_directory_exists(output_dir);

	lcma_file_metadata_t *metadatas = malloc(hdr.file_count * sizeof(lcma_file_metadata_t));
	for (int i = 0; i < hdr.file_count; i++)
	{
		lcma_file_metadata_t file_metadata = {0};
		if (read_metadata(in, &file_metadata) != TRUE)
		{
			for (int j = 0; j < i; j++) {
				free(metadatas[j].name);
			}
			fclose(in);
			free(metadatas);
			return FAIL_INVALID_ARCHIVE_PAYLOAD;
		}
		
		metadatas[i] = file_metadata;
	}

	for (int i = 0; i < hdr.file_count; i++)
	{
		char *file_output = join_path(output_dir, metadatas[i].name); 
		if (!file_output) {
			for (int j = 0; j < hdr.file_count; j++) {
				free(metadatas[j].name);
			}
			free(metadatas);
			fclose(in);
			return FAIL_CANT_READ_DATA;
		}
		
		FILE *out = fopen(file_output, "wb");
		if (!out) {
			free(file_output);
			for (int j = 0; j < hdr.file_count; j++) {
				free(metadatas[j].name);
			}
			free(metadatas);
			fclose(in);
			return FAIL_CANT_OPEN_FILE;
		}
		
		stream_copy(in, out);
		fclose(out);
		free(file_output);
	}

	for (int i = 0; i < hdr.file_count; i++) {
		free(metadatas[i].name);
	}
	free(metadatas);
	fclose(in);
	return SUCCESS;
}

int read_metadata(FILE *in, lcma_file_metadata_t *m) {
    if (fread(&m->name_length, sizeof(m->name_length), 1, in) != 1)
		return FAIL_CANT_READ_DATA;

    char *name = malloc(m->name_length + 1);
    if (!name) return FAIL_CANT_READ_DATA;

    if (fread(name, 1, m->name_length, in) != m->name_length) {
        free(name);
        return FAIL_CANT_READ_DATA;
    }
    name[m->name_length] = '\0';
	m->name = name;

	if (fread(&m->size, sizeof(m->size), 1, in) != 1) {
		free(name);
		return FAIL_CANT_READ_DATA;
	}

    return SUCCESS;
}
