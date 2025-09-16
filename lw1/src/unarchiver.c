#include "unarchiver.h"
#include "archive_format.h"
#include <stdio.h>

int archive_extract(const char *archive_path, const char *output_dir, const char *password) {
	(void)password;
	FILE *in = fopen(archive_path, "rb");
	if (!in) return -1;

	lcma_archive_header_t hdr = {0};
	if (fread(&hdr, sizeof(hdr), 1, in) != 1) {
		fclose(in);
		return -2;
	}

	if (hdr.magic != LCMA_MAGIC) {
		fclose(in);
		return -3;
	}

	(void)output_dir;

	fclose(in);
	return 0;
}


