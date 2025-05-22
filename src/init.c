#include "clide.h"

static void signal_handler(int signum) {
	if (stdscr != NULL) {  /* Checks if ncurses has been initialized */
		endwin();  /* Exit curses mode gracefully */
		exit(EXIT_FAILURE);
	}
}

static void apply_color_theme(void) {
	if (!strcmp(config.theme, "dark")) {
		init_pair(10, COLOR_WHITE, COLOR_BLACK);
		attron(COLOR_PAIR(10));
		bkgd(COLOR_PAIR(10));
	} else if (!strcmp(config.theme, "bright")) {
		init_pair(10, COLOR_BLACK, COLOR_WHITE);
		attron(COLOR_PAIR(10));
		bkgd(COLOR_PAIR(10));
	}
	/* Invalid color themes are ignored */
}

void initialize_clide(void) {
	static mmask_t mouse_mask = (
		BUTTON1_PRESSED
		| BUTTON1_RELEASED
		| BUTTON2_CLICKED
		| BUTTON4_PRESSED /* Mousewheel */
		| BUTTON5_PRESSED /* Mousewheel */
	);
	initscr();  /* initialize ncurses */
	raw();  /* disable tty buffering */
	noecho();  /* disable tty input echoing */
	keypad(stdscr, TRUE);  /* enable extended key support */
	mousemask(mouse_mask, NULL);  /* activate mouse event filter */
	mouseinterval(10);  /* set mouse event trigger interval */
	signal(SIGSEGV, signal_handler);  /* ensure graceful exit on event */
	signal(SIGKILL, signal_handler);  /* ensure graceful exit on event */
	signal(SIGABRT, signal_handler);  /* ensure graceful exit on event */
	if (has_colors()) {
		start_color();
		init_color(COLOR_WHITE, 1000, 1000, 1000);
		apply_color_theme();
	}
	update_terminal_dimensions();
}

void quit_clide(void) {
	clear_clipboard();
	endwin();
}
