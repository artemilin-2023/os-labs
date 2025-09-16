#include "password.h"
#include <stdlib.h>
#include <string.h>

int password_init(lcma_password_t *pw, const char *input) {
	if (!pw) return -1;
	pw->data = NULL;
	pw->length = 0;
	if (!input) return 0;
	size_t len = strlen(input);
	char *buf = (char*)malloc(len + 1);
	if (!buf) return -2;
	memcpy(buf, input, len + 1);
	pw->data = buf;
	pw->length = len;
	return 0;
}

void password_destroy(lcma_password_t *pw) {
	if (!pw || !pw->data) return;
	volatile char *p = (volatile char*)pw->data;
	for (size_t i = 0; i < pw->length; ++i) p[i] = 0;
	free(pw->data);
	pw->data = NULL;
	pw->length = 0;
}


