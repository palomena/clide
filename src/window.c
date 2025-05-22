#include "clide.h"

struct TerminalWindow window;
struct Cursor cursor;

void update_terminal_dimensions(void) {
	getmaxyx(stdscr, window.height, window.width);
	getbegyx(stdscr, window.y, window.x);
	editor.x = window.x;
	editor.y = window.y + 1;  /* space for title bar */
	editor.width = window.width;
	editor.height = window.height - 2;  /* minus title bar and status bar */
}

static unsigned short cursor_index = 0;
static struct Cursor cursor_stack[3];

void push_cursor(void) {
	assert (cursor_index < lengthof(cursor_stack));
	cursor_stack[cursor_index] = cursor;
	cursor_index++;
}

void pop_cursor(void) {
	assert (cursor_index > 0);
	cursor_index--;
	cursor = cursor_stack[cursor_index];
	move(cursor.y, cursor.x);
}

size_t normalize(size_t index) {
	assert (index > 0);
	return index - 1;
}

static int map_line_number_to_cursor_position(size_t line_number) {
	return line_number - editor.line_offset + editor.y;
}

static int map_column_number_to_cursor_position(size_t column_number) {
	int x = 0;
	for (size_t i = editor.column_offset; i < column_number; i++) {
		int tabstep = config.tabsize - i % config.tabsize;
		x += text_of(current_line())[i] == '\t' ? tabstep : 1;
	}
	return min(x, editor.width);
}

void update_cursor(size_t line_number, size_t column_number) {
	cursor.y = map_line_number_to_cursor_position(line_number);
	cursor.x = map_column_number_to_cursor_position(column_number);
	move(cursor.y, cursor.x);
	print_status_bar();  /* Update cursor position widget */
}

static void ensure_visible_by_vertical_scrolling(void) {
	if (editor.line_offset > 0 and normalize(editor.line) < editor.line_offset) {
		editor.line_offset = normalize(editor.line);
		print_page();
	} else if (normalize(editor.line) >= editor.line_offset+editor.height) {
		editor.line_offset = 1+normalize(editor.line)-editor.height;
		print_page();
	}
}

static void ensure_visible_by_horizontal_scrolling(void) {
	if (editor.column_offset > 0 and normalize(editor.column) < editor.column_offset) {
		editor.column_offset = normalize(editor.column); /* scroll left */
		print_page();
	} else if (normalize(editor.column) >= editor.column_offset+editor.width) {
		editor.column_offset = 1+normalize(editor.column)-editor.width; /* scroll right */
		print_page();
	}
}

void update_current_cursor(void) {
	editor.column = min(editor.column, 1+current_line()->length);
	ensure_visible_by_vertical_scrolling();
	ensure_visible_by_horizontal_scrolling();
	update_cursor(normalize(editor.line), normalize(editor.column));
}

static size_t map_cursor_position_to_line_number(int y) {
	return min(
		1 + editor.line_offset + y - editor.y,
		editor.document->num_lines
	);
}

static size_t map_cursor_position_to_column_number(int x) {
	int cursor = 0;
	for (size_t i = 0; i < current_line()->length; i++) {
		int tabstep = config.tabsize - i % config.tabsize;
		cursor += char_at(normalize(editor.line), i) == '\t' ? tabstep : 1;
		if (cursor > x) {
			return 1+i;
		}
	}
	return 1+current_line()->length;
}

void update_cursor_reverse(int y, int x) {
	editor.line = map_cursor_position_to_line_number(y);
	editor.column = map_cursor_position_to_column_number(x);
	update_current_cursor();
}

void print_title_bar(void) {
	push_cursor();
	move(window.y, window.x);
	clrtoeol();
	chgat(-1, A_REVERSE, 10, NULL);
	attron(A_REVERSE | A_BOLD);
	printw("\tclide 0.1\t");
	attroff(A_BOLD);
	printw("File %s%c", editor.document->path, editor.was_modified ? '*' : ' ');
	attroff(A_REVERSE);
	pop_cursor();
}

void print_status_bar(void) {
	push_cursor();
	move(window.height - 1, window.x);
	clrtoeol();
	chgat(-1, A_REVERSE, 10, NULL);
	attron(A_REVERSE);
	printw(
		"  Term %dx%d | Ln %zu/%zu | Col %u/%u | Off %zu,%zu",
		window.width, window.height,
		editor.line, editor.document->num_lines,
		editor.column, current_line()->length + 1,
		editor.line_offset, editor.column_offset
	);
	attroff(A_REVERSE);
	pop_cursor();
}