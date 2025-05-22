#include "clide.h"

struct DocumentEditor editor;

void signal_modification(void) {
	editor.was_modified = true;
	print_title_bar();
}

void open_document_editor(void) {
	editor.document = open_document(config.input);
	editor.line = 1;
	editor.column = 1;
	editor.line_offset = 0;
	editor.column_offset = 0;
	update_cursor(editor.line-1, editor.column-1);
	print_title_bar();
	print_page();
}

void close_document_editor(void) {
	close_document(editor.document);
}

struct Line** line_at(size_t index) {
	assert (index >= 0);
	assert (index < editor.document->num_lines);
	return &editor.document->lines[index];
}

int char_at(size_t line_number, size_t index) {
	return text_of(*line_at(line_number))[index];
}

struct Line* current_line(void) {
	return *line_at(editor.line - 1);
}

int current_character(int n) {
	assert (normalize(editor.column)+n >= 0);
	assert (normalize(editor.column)+n < current_line()->length);
	return text_of(current_line())[normalize(editor.column)+n];
}

static bool line_is_visible(size_t line_number) {
	return (
		line_number >= editor.line_offset and
		line_number < editor.line_offset + editor.height
	);
}

void print_line(size_t index) {
	assert (line_is_visible(index));
	struct Line *line = *line_at(index);
	push_cursor();
	update_cursor(index, 0);
	int curx = editor.x;
	clrtoeol();
	if (editor.column_offset < line->length) /* nobrackets */
	for (size_t i = 0; i < line->length - editor.column_offset; i++) {
		if (curx >= editor.width) break;
		if (active_selection()) highlight_selection(index, i);
		addch(text_of(line)[editor.column_offset + i]);
		curx++;
	}
	pop_cursor();
}

void print_current_line(void) {
	print_line(normalize(editor.line));
}

void print_page(void) {
	size_t end = min(editor.document->num_lines, editor.height);
	int y = editor.y;
	for (size_t i = 0; i < end; i++) {
		if (editor.line_offset + i < editor.document->num_lines) {
			print_line(editor.line_offset + i);
		} else {
			move(y, 0);
			clrtoeol();
		}
		y++;
	}
	refresh();
}

void insert_character_at_current_position(int ch) {
	insert_character(
		line_at(normalize(editor.line)),
		normalize(editor.column),
		ch
	);
	signal_modification();
}

void delete_character_at_current_position(void) {
	if (normalize(editor.column) < current_line()->length) {
		remove_character(
			line_at(normalize(editor.line)),
			normalize(editor.column)
		);
	}
	signal_modification();
}

void merge_with_next_line(void) {
	if (normalize(editor.line) < editor.document->num_lines) {
		struct Line *next_line = *line_at(1+normalize(editor.line));
		append_string(
			line_at(normalize(editor.line)),
			text_of(next_line)
		);
		remove_line(&editor.document, 1+normalize(editor.line));
		free_line(next_line);
		signal_modification();
	}
}

void insert_line_at_current_position(void) {
	size_t lineno = 1+normalize(editor.line);
	insert_line(&editor.document, lineno, create_line());
	append_string(line_at(lineno), text_of(current_line()) + normalize(editor.column));
	clear_line(current_line(), normalize(editor.column));
	current_line()->length = normalize(editor.column);
	signal_modification();
}
