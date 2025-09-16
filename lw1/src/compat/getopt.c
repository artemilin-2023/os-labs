#include "getopt.h"

char *optarg = 0;
int optind = 1;
int opterr = 1;
int optopt = '?';

int getopt(int argc, char * const argv[], const char *optstring) {
	static int optpos = 1;
	if (optind >= argc) return -1;
	const char *arg = argv[optind];
	if (arg[0] != '-' || arg[1] == '\0') return -1;
	if (strcmp(arg, "--") == 0) { optind++; return -1; }
	char opt = arg[optpos];
	const char *spec = strchr(optstring, opt);
	if (!spec) { optopt = opt; if (arg[++optpos] == '\0') { optind++; optpos = 1; } return '?'; }
	if (spec[1] == ':') {
		if (arg[optpos + 1] != '\0') {
			optarg = (char*)&arg[optpos + 1];
			optind++; optpos = 1;
		} else if (optind + 1 < argc) {
			optarg = argv[++optind];
			optind++; optpos = 1;
		} else {
			optopt = opt;
			if (opterr) {}
			return ':';
		}
	} else {
		if (arg[++optpos] == '\0') { optind++; optpos = 1; }
	}
	return opt;
}


