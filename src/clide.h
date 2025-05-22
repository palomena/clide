/******************************************************************************
 * clide - command line interface document editor
 * Copyright (c) 2017-2025 Niklas Benfer
 * Licensed under the terms of the MIT License.
 *****************************************************************************/

#ifndef CLIDE_H
#define CLIDE_H

/******************************************************************************
 * MARK: Dependencies
 * Requires ISO C90, hosted implementation of C standard library,
 * libncurses-dev, POSIX getopt.
 * Other than that it is a self-contained single-source file program.
 *****************************************************************************/

#include <assert.h>
#include <ctype.h>
#include <getopt.h>
#include <iso646.h>
#include <ncurses.h>
#include <regex.h>
#include <signal.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/******************************************************************************
 * MARK: Config
 *****************************************************************************/

struct Config {
	const char *input;
	const char *theme;
	int tabsize;
	bool stream_file_contents;
};

/**
 * Global configuration record
 */
extern struct Config config;

/**
 * Parses command line arguments, overrides the global config where necessary
 */
extern void parse_arguments(int argc, char *argv[]);

/******************************************************************************
 * MARK: Line
 *****************************************************************************/

/**
 * A character array for representing lines in a document.
 * Line length is limited to UINT16_MAX (65535) because longer
 * lines do not make much sense when dealing with text documents.
 * The actual string of characters begins after sizeof(struct Line).
 */
struct Line {
	uint16_t length;
	uint16_t capacity;
};

/**
 * Retrieves the string of characters from the given line.
 */
extern char* text_of(struct Line *line);

/**
 * Clears a line from the given start position to line end.
 */
extern void clear_line(struct Line *line, size_t start);

/**
 * Creates a new empty line.
 */
extern struct Line* create_line(void);

/**
 * Frees the allocated memory of the given line.
 */
extern void free_line(struct Line *line);

/**
 * Inserts a character at the given position into the line.
 */
extern void insert_character(struct Line **lineptr, size_t position, int ch);

/**
 * Appends a character to the line.
 */
extern void append_character(struct Line **lineptr, int ch);

/**
 * Appends a string of text to the given line.
 */
extern void append_string(struct Line **lineptr, const char *text);

/**
 * Removes the character at the given position from the line.
 */
extern void remove_character(struct Line **lineptr, size_t position);

/**
 * Reads a line from a file stream.
 */
extern struct Line* read_line_from_file(FILE *fp);

/******************************************************************************
 * MARK: Document
 *****************************************************************************/

/**
 * Models a text document
 */
struct TextDocument {
	char *path;
	struct Line **lines;
	size_t num_lines;
	size_t capacity;
};

/**
 * 
 */
extern struct TextDocument* open_document(const char *path);

/**
 *
 */
extern void close_document(struct TextDocument *document);

/**
 *
 */
extern void save_document(struct TextDocument *document);

/**
 *
 */
extern void insert_line(struct TextDocument **docptr, size_t index, struct Line *line);

/**
 *
 */
extern void append_line(struct TextDocument **docptr, struct Line *line);

/**
 *
 */
extern void remove_line(struct TextDocument **docptr, size_t index);

/******************************************************************************
 * MARK: Clipboard
 *****************************************************************************/

/**
 * Clears the clipboard contents and frees allocated clipboard memory.
 */
extern void clear_clipboard(void);

/**
 * Copies the currently selected text to the clipboard.
 */
extern void copy_current_selection(void);

/**
 * Copies the currently selected text to the clipboard
 * and deletes it from the editor window.
 */
extern void cut_current_selection(void);

/**
 * Pastes the clipboard contents into the editor window.
 */
extern void paste_clipboard(void);

/**
 *
 */
extern bool active_selection(void);

/**
 *
 */
extern void begin_selection(void);

/**
 *
 */
extern void invalidate_selection(void);

/**
 *
 */
extern void update_selection(void);

/**
 *
 */
extern void highlight_selection(size_t line, size_t column);

/******************************************************************************
 * MARK: Init
 *****************************************************************************/

/**
 * Initializes ncurses and sets up the editor window.
 */
extern void initialize_clide(void);

/**
 * De-initializes ncurses and tears down the editor window.
 */
extern void quit_clide(void);

/******************************************************************************
 * MARK: Window
 *****************************************************************************/

struct TerminalWindow {
	int x, y;
	int width, height;
};

extern struct TerminalWindow window;

/**
 * Updates the dimensions upon terminal window resize.
 */
extern void update_terminal_dimensions(void);

/**
 * Models the terminal window cursor
 */
struct Cursor {
	int x, y;
};

extern struct Cursor cursor;

/**
 * Stores the cursor position
 */
extern void push_cursor(void);

/**
 * Restores the cursor position
 */
extern void pop_cursor(void);

/**
 *
 */
extern void update_cursor(size_t line_number, size_t column_number);

/**
 *
 */
extern void update_current_cursor(void);

/**
 *
 */
extern void update_cursor_reverse(int y, int x);

/**
 *
 */
extern void print_title_bar(void);

/**
 *
 */
extern void print_status_bar(void);

/**
 * Subtracts 1 from the index.
 * Applicable to line numbers and column numbers, both
 * of which logically start at 1.
 */
extern size_t normalize(size_t index);

/******************************************************************************
 * MARK: Editor
 *****************************************************************************/

struct DocumentEditor {
	struct TextDocument *document;
	int x, y;
	int width, height;
	size_t line_offset;
	size_t column_offset;
	size_t line;
	uint16_t column;
	bool was_modified;
};

extern struct DocumentEditor editor;

/**
 *
 */
extern void signal_modification(void);

/**
 *
 */
extern void open_document_editor(void);

/**
 *
 */
extern void close_document_editor(void);

/**
 *
 */
extern struct Line** line_at(size_t index);

/**
 *
 */
extern int char_at(size_t line_number, size_t index);

/**
 *
 */
extern struct Line* current_line(void);

/**
 *
 */
extern int current_character(int n);

/**
 *
 */
extern void print_title_bar(void);

/**
 *
 */
extern void print_status_bar(void);

/**
 *
 */
extern void print_line(size_t index);

/**
 *
 */
extern void print_current_line(void);

/**
 *
 */
extern void print_page(void);

/**
 *
 */
extern void insert_character_at_current_position(int ch);

/**
 *
 */
extern void delete_character_at_current_position(void);

/**
 *
 */
extern void merge_with_next_line(void);

/**
 *
 */
extern void insert_line_at_current_position(void);

/******************************************************************************
 * MARK: Actions
 *****************************************************************************/

extern bool can_move_left(void);
extern bool can_move_right(void);
extern bool can_move_up(void);
extern bool can_move_down(void);
extern bool can_scroll(int n);
extern void move_left(void);
extern void move_right(void);
extern void move_up(void);
extern void move_down(void);
extern void move_to_end_of_line(void);
extern void move_to_beginning_of_line(void);
extern void seek_left(void);
extern void seek_right(void);
extern void scroll_page(int n);
extern void move_to_next_page(void);
extern void move_to_previous_page(void);

/******************************************************************************
 * MARK: Dialog
 *****************************************************************************/

/**
 * Opens an interactive dialog window for the user to enter a line number into.
 * The return value is the line number entered by the user.
 */
extern size_t launch_goto_line_dialog(void);

/**
 * Opens an interactive dialog window for the user to enter a text into.
 * The return value is the next line number within the document where
 * the given text occurs in.
 */
extern size_t launch_find_text_dialog(void);

/**
 * Opens an interactive dialog window for the user to enter a search text
 * and a replacement text into.
 */
extern void launch_replace_text_dialog(void);

/******************************************************************************
 * MARK: Input
 *****************************************************************************/

/**
 *
 */
extern bool handle_input(int key);

/******************************************************************************
 * MARK: Utils
 *****************************************************************************/

/* Returns the length of an array at compile-time */
#define lengthof(x) (sizeof(x)/sizeof((x))[0])

/* Swaps the values of two variables a and b */
#define swap(a, b) do { \
	int c = (a); \
	(a) = (b); \
	(b) = c; \
} while (false);

/**
 * Returns the minimum size
 */
extern size_t min(size_t a, size_t b);

/**
 * Duplicates the given string
 */
extern char* strdup(const char *text);

#endif /* CLIDE_H */
