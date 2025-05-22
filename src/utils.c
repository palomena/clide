#include "clide.h"

size_t min(size_t a, size_t b) {
	return a <= b ? a : b;
}

char* strdup(const char *text) {
	size_t length = strlen(text);
	char *string = malloc(length + 1);
	memcpy(string, text, length);
	string[length] = '\0';
	return string;
}
