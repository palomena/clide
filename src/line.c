#include "clide.h"

char* text_of(struct Line *line) {
	return (char*)(line + 1);
}

static struct Line* allocate_line_memory(struct Line *line, size_t n) {
	line = realloc(line, sizeof(*line) + sizeof(char) * n);
	line->capacity = n;
	return line;
}

void clear_line(struct Line *line, size_t start) {
	memset(text_of(line) + start, 0, line->capacity - start);
}

struct Line* create_line(void) {
	static const size_t default_capacity = 128;
	struct Line *line = allocate_line_memory(NULL, default_capacity);
	clear_line(line, 0);
	line->length = 0;
	return line;
}

void free_line(struct Line *line) {
	clear_line(line, 0);
	free(line);
}

static bool line_is_exhausted(struct Line *line) {
	return line->length >= line->capacity;
}

static void extend_line_capacity(struct Line **lineptr) {
	*lineptr = allocate_line_memory(*lineptr, 2 * (*lineptr)->capacity);
	clear_line(*lineptr, (*lineptr)->length);  /* Clear extended section */
}

void insert_character(struct Line **lineptr, size_t position, int ch) {
	assert (position <= (*lineptr)->length);
	(*lineptr)->length++;
	if (line_is_exhausted(*lineptr)) {
		extend_line_capacity(lineptr);
	}
	memmove(  /* Make room for the new character */
		text_of(*lineptr) + position + 1,
		text_of(*lineptr) + position,
		(*lineptr)->length - position
	);
	text_of(*lineptr)[position] = ch;
}

void append_character(struct Line **lineptr, int ch) {
	insert_character(lineptr, (*lineptr)->length, ch);
}

void append_string(struct Line **lineptr, const char *text) {
	while (*text) {
		append_character(lineptr, *text++);
	}
}

void remove_character(struct Line **lineptr, size_t position) {
	assert ((*lineptr)->length > 0);
	assert (position < (*lineptr)->length);
	memmove(  /* Overwrite the character at position */
		text_of(*lineptr) + position,
		text_of(*lineptr) + position + 1,
		(*lineptr)->length - position
	);
	(*lineptr)->length--;
	/* Null-terminator automatically moves to new length in memmove */
}

struct Line* read_line_from_file(FILE *fp) {
	struct Line *line = create_line();
	int ch;
	for (ch = fgetc(fp); ch != '\n' and ch != EOF; ch = fgetc(fp)) {
		append_character(&line, ch);
	}
	return line;
}