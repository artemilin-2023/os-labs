#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <getopt.h>

#include "archiver.h"
#include "unarchiver.h"
#include "password.h"

static void print_usage(const char *prog) {
	fprintf(stderr, "Usage: %s [-c input -o archive] | [-x archive -d out] [-p password]\n", prog);
	fprintf(stderr, "  -c <path>    input path to archive\n");
	fprintf(stderr, "  -o <file>    output archive file\n");
	fprintf(stderr, "  -x <file>    extract archive file\n");
	fprintf(stderr, "  -d <path>    output directory for extraction\n");
	fprintf(stderr, "  -p <pass>    password (optional)\n");
}

int main(int argc, char **argv) {
	int opt;
	const char *input = NULL;
	const char *archive = NULL;
	const char *extract = NULL;
	const char *outdir = NULL;
	const char *pass = NULL;

	while ((opt = getopt(argc, argv, "c:o:x:d:p:")) != -1) {
		switch (opt) {
		case 'c': input = optarg; break;
		case 'o': archive = optarg; break;
		case 'x': extract = optarg; break;
		case 'd': outdir = optarg; break;
		case 'p': pass = optarg; break;
		default:
			print_usage(argv[0]);
			return 1;
		}
	}

	if ((input && extract) || (!input && !extract)) {
		print_usage(argv[0]);
		return 1;
	}

	int rc = 0;
	lcma_password_t pw; password_init(&pw, pass);
	if (input) {
		if (!archive) { print_usage(argv[0]); return 1; }
		rc = archive_create(input, archive, &pw);
	} else {
		if (!outdir) { print_usage(argv[0]); return 1; }
		rc = archive_extract(extract, outdir, &pw);
	}
	password_destroy(&pw);
	printf("done");
	return rc == 0 ? 0 : 1;
}

