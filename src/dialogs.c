#include "clide.h"

static unsigned int digits(size_t n) {
	if (n < 0) return digits ((n == 0) ? SIZE_MAX: -n);
	if (n < 10) return 1;
	return 1 + digits (n / 10);
}

static void prompt(WINDOW *form, const char *text, char *value, unsigned int size) {
	wattron(form, A_REVERSE);
	mvwprintw(form, 1, 2, text);
	wattroff(form, A_REVERSE);
	wrefresh(form);
	echo();
	mvwgetnstr(form, 2, 2, value, size);
	noecho();
	wclear(form);
	wrefresh(form);
	delwin(form);
}

size_t launch_goto_line_dialog(void) {
	const int width = 4+digits(SIZE_MAX);
	const int height = 4;
	char value[digits(SIZE_MAX)];
	size_t result = 1;
	WINDOW *form = newwin(height, width, editor.height/2, editor.width/2-width/2);
	box(form, 0, 0);
	prompt(form, "Goto line", value, sizeof(value));
	long line = strtol(value, 0, 10);
	if (line > editor.document->num_lines)
		result = editor.document->num_lines;
	else if (line < 1)
		result = 1;
	else
		result = line;
	return result;
}

size_t launch_find_text_dialog(void) {
	const int width = 4+digits(SIZE_MAX);
	const int height = 6;
	char value[128];
	WINDOW *form = newwin(height, width, editor.height/2, editor.width/2-width/2);
	box(form, 0, 0);
	prompt(form, "Search", value, sizeof(value));
	return 0;
}

void launch_replace_text_dialog(void) {
	const int width = 4+digits(SIZE_MAX);
	const int height = 6;
	char value[128];
	WINDOW *form = newwin(height, width, editor.height/2, editor.width/2-width/2);
	box(form, 0, 0);
	prompt(form, "Search", value, sizeof(value));
}
