#include "clide.h"

static bool document_is_exhausted(struct TextDocument *doc) {
	return doc->num_lines >= doc->capacity;
}

static struct TextDocument* allocate_document_memory(struct TextDocument *doc, size_t n) {
	doc = realloc(doc, sizeof(*doc) + sizeof(*doc->lines) * n);
	doc->capacity = n;
	doc->lines = (void*)(doc + 1);
	return doc;
}

static void extend_document_capacity(struct TextDocument **docptr) {
	*docptr = allocate_document_memory(*docptr, 2 * (*docptr)->capacity);
}

void insert_line(struct TextDocument **docptr, size_t index, struct Line *line) {
	assert (index <= (*docptr)->num_lines);
	(*docptr)->num_lines++;
	if (document_is_exhausted(*docptr)) {
		extend_document_capacity(docptr);
	}
	memmove(  /* Make room for the new line */
		(*docptr)->lines + index + 1,
		(*docptr)->lines + index,
		sizeof((*docptr)->lines) * ((*docptr)->num_lines - index)
	);
	(*docptr)->lines[index] = line;
}

void append_line(struct TextDocument **docptr, struct Line *line) {
	insert_line(docptr, (*docptr)->num_lines, line);
}

void remove_line(struct TextDocument **docptr, size_t index) {
	assert ((*docptr)->num_lines > 0);
	assert (index < (*docptr)->num_lines);
	memmove(  /* Overwrite the line at index */
		(*docptr)->lines + index,
		(*docptr)->lines + index + 1,
		sizeof((*docptr)->lines) * ((*docptr)->num_lines - index)
	);
	(*docptr)->num_lines--;
}

struct TextDocument* open_document(const char *path) {
	static const size_t default_capacity = 128; /* num lines */
	struct TextDocument *doc = allocate_document_memory(NULL, default_capacity);
	FILE *fp = fopen(path, "rt");
	doc->path = strdup(path);
	doc->num_lines = 0;
	if (fp != NULL) {
		while (not feof(fp)) {
			append_line(&doc, read_line_from_file(fp));
		}
		fclose(fp);
	} else {
		append_line(&doc, create_line());
	}
	return doc;
}

void close_document(struct TextDocument *doc) {
	size_t index;
	for (index = 0; index < doc->num_lines; index++) {
		free_line(doc->lines[index]);
	}
	free(doc->path);
	free(doc);
}

void save_document(struct TextDocument *doc) {
	FILE *fp = fopen(doc->path, "wt");
	if (fp != NULL) {
		size_t i;
		for (i = 0; i < doc->num_lines; i++) {
			fputs(text_of(doc->lines[i]), fp);
			fputc('\n', fp);
		}
		fclose(fp);
	}
}