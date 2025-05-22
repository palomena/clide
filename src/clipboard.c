#include "clide.h"

/**
 * All line and column numbers stored here are NORMALIZED!
 */
struct Selection {
	size_t start_line;
	size_t end_line;
	uint16_t start_column;
	uint16_t end_column;
	bool is_active;
};

static struct Selection selection;

static struct Line* clipboard = NULL;

void clear_clipboard(void) {
    if (clipboard != NULL) {
        free_line(clipboard);
        clipboard = NULL;
    }
}

void copy_current_selection(void) {
    if (selection.is_active) {
        clear_clipboard();
        clipboard = create_line();
        for (size_t line = selection.start_line; line <= selection.end_line; line++) {
            size_t col_start = line == selection.start_line ? selection.start_column : 0;
			size_t col_end = line == selection.end_line ? selection.end_column : (*line_at(line))->length;
            for (size_t column = col_start; column < col_end; column++) {
                append_character(&clipboard, char_at(line, column));
            }
            if (line < selection.end_line) {
                append_character(&clipboard, '\n');
            }
        }
    }
}

static void delete_current_selection(void) {

}

void cut_current_selection(void) {
    copy_current_selection();
    delete_current_selection();
}

void paste_clipboard(void) {
    if (clipboard != NULL) {
        for (size_t i = 0; i < clipboard->length; i++) {
            if (text_of(clipboard)[i] == '\n') {
                struct Line *line = create_line();
                insert_line(&editor.document, 1+normalize(editor.line), line);
                append_string(&line, text_of(current_line()) + normalize(editor.column));
                clear_line(current_line(), normalize(editor.column));
                current_line()->length = normalize(editor.column);
                editor.line++;
                editor.column = 1;
                continue;
            }
            insert_character_at_current_position(text_of(clipboard)[i]);
            editor.column++;
        }
        print_page();
        update_current_cursor();
    }
}

bool active_selection(void) {
    return selection.is_active;
}

void begin_selection(void) {
    invalidate_selection();
    selection.is_active = true;
    selection.start_line = normalize(editor.line);
    selection.start_column = normalize(editor.column);
    selection.end_line = selection.start_line;
    selection.end_column = selection.start_column;
}

static void repaint_selected_area(void) {
    for (size_t line = selection.start_line; line <= selection.end_line; line++) {
        print_line(line);
    }
}

void invalidate_selection(void) {
    if (selection.is_active) {
        selection.is_active = false;
        repaint_selected_area();  /* Repaint to get rid of selection marks */
    }
}

void update_selection(void) {
    assert (selection.is_active);
    size_t previous_end_line = selection.end_line;
    selection.end_line = normalize(editor.line);
    selection.end_column = normalize(editor.column);
    /* Ensure selection start is always smaller than selection end */
    if (selection.end_line == selection.start_line and selection.end_column < selection.start_column) {
		swap(selection.end_column, selection.start_column);
	} else if (selection.end_line < selection.start_line) {
		swap(selection.end_line, selection.start_line);
		swap(selection.end_column, selection.start_column);
	}
    /* Invalidate selection if nothing is selected */
	if (selection.end_line == selection.start_line and selection.end_column == selection.start_column) {
		invalidate_selection();
	} else {  /* Otherwise, repaint to show selection */
        repaint_selected_area();
        /* Repair affected lines, if selected area has shrunk */
        for (size_t line = 1+selection.end_line; line <= previous_end_line; line++) {
            print_line(line);
        }
    }
}

void highlight_selection(size_t line, size_t column) {
	if ((
			line == selection.start_line and
			line == selection.end_line and
			column>= selection.start_column and
			column<= selection.end_column
		) or (
			line == selection.start_line and
			line < selection.end_line and
			column>= selection.start_column
		) or (
			line > selection.start_line and
			line < selection.end_line
		) or (
			line > selection.start_line and
			line == selection.end_line and
			column<= selection.end_column
		)
	) {
		attron(A_REVERSE);
	} else {
		attroff(A_REVERSE);
	}
}
