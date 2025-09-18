#include "unarchiver.h"
#include "archive_format.h"
#include "file_metadata.h"
#include <stdio.h>
#include "constants.h"
#include "fileutils.h"
#include "stdlib.h"

int read_metadata(FILE *in, lcma_file_metadata_t *m);

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

	create_dirs(output_dir);

	lcma_file_metadata_t *metadatas = malloc(hdr.file_count * sizeof(lcma_file_metadata_t));
	for (int i = 0; i < hdr.file_count; i++)
	{
		lcma_file_metadata_t file_metadata = {0};
		if (read_metadata(in, &file_metadata) != SUCCESS)
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
		const char *file_output = join_path(output_dir, metadatas[i].name);
		if (!file_output) {
			for (int j = 0; j < hdr.file_count; j++) {
				free(metadatas[j].name);
			}
			free(metadatas);
			fclose(in);
			return FAIL_CANT_READ_DATA;
		}
		
		create_dirs(file_output);
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
		
		size_t size = metadatas[i].size;
		char buffer[4096];
		while (size > 0)
		{
			// читаем не больше, чем осталось или размер буфера
			size_t to_read = (size < sizeof(buffer))
				? size
				: sizeof(buffer);

			size_t readed = fread(buffer, 1, to_read, in);
			if (readed == 0)
				break;

			fwrite(buffer, 1, readed, out);
			size -= readed;
		}

		fclose(out);
		free(file_output);
	}

	free(metadatas);
	fclose(in);
	return SUCCESS;
}

int read_metadata(FILE *in, lcma_file_metadata_t *m) {
    if (fread(&m->name_length, sizeof(m->name_length), 1, in) != 1)
		return FAIL_CANT_READ_DATA;

	if (fread(&m->size, sizeof(m->size), 1, in) != 1)
		return FAIL_CANT_READ_DATA;

    m->name = malloc(m->name_length + 1);
    if (!m->name) return FAIL_CANT_READ_DATA;

    if (fread(m->name, 1, m->name_length, in) != m->name_length) {
        free(m->name);
        return FAIL_CANT_READ_DATA;
    }
    m->name[m->name_length] = '\0';

    return SUCCESS;
}
