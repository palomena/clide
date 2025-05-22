#include "clide.h"

bool can_move_left(void) {
	return normalize(editor.column) > 0;
}

bool can_move_right(void) {
	return normalize(editor.column) < current_line()->length;
}

bool can_move_up(void) {
	return normalize(editor.line) > 0;
}

bool can_move_down(void) {
	return editor.line < editor.document->num_lines;
}

static bool can_scroll_up(int n) {
	return editor.line_offset >= n;
}

static bool can_scroll_down(int n) {
	return editor.line_offset + n + editor.height <= editor.document->num_lines;
}

bool can_scroll(int n) {
	if (n > 0)      return can_scroll_down(n);
	else if (n < 0) return can_scroll_up(abs(n));
	return false;
}

void move_left(void) {
	assert (can_move_left());
	editor.column--;
	update_current_cursor();
}

void move_right(void) {
	assert (can_move_right());
	editor.column++;
	update_current_cursor();
}

void move_up(void) {
	assert (can_move_up());
	editor.line--;
	update_current_cursor();
}

void move_down(void) {
	assert (can_move_down());
	editor.line++;
	update_current_cursor();
}

void move_to_end_of_line(void) {
	while (can_move_right()) {
		move_right();
	}
}

void move_to_beginning_of_line(void) {
	while (can_move_left()) {
		move_left();
	}
}

void seek_left(void) {
	if (can_move_left()) {
		move_left();
	}
	while (can_move_left() and isalnum(current_character(0))) {
		move_left();
	}
}

void seek_right(void) {
	if (can_move_right()) {
		move_right();
	}
	while (can_move_right() and isalnum(current_character(0))) {
		move_right();
	}
}

void scroll_page(int n) {
	editor.line_offset += n;
	print_page();
}

void move_to_next_page(void) {
	if (can_scroll(editor.height)) {
		size_t lineno = editor.line - editor.line_offset;
		scroll_page(editor.height);
		editor.line = lineno+editor.line_offset;
	} else if (editor.document->num_lines - editor.line_offset > editor.height) {  /* goto end of document */
		scroll_page(editor.document->num_lines - editor.line_offset - editor.height);
		editor.line = editor.document->num_lines;
		move_to_end_of_line();
	} else {
		editor.line = editor.document->num_lines;
		move_to_end_of_line();
	}
	update_current_cursor();
}

void move_to_previous_page(void) {
	if (can_scroll(-editor.height)) {
		size_t lineno = editor.line - editor.line_offset;
		scroll_page(-editor.height);
		editor.line = lineno+editor.line_offset;
	} else {  /* goto beginning of document */
		scroll_page(-editor.line_offset);
		editor.line = 1;
		move_to_beginning_of_line();
	}
	update_current_cursor();
}
