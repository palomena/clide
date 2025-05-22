#include "clide.h"

/* Bitmask for control-key combinations (e.g. to catch CTRL+C) */
#define CTRL(c) ((c) & 0x1F)

/* Additional key event definitions */
#define KEY_CTRL_LEFT         550
#define KEY_CTRL_SHIFT_LEFT   551
#define KEY_CTRL_RIGHT        565
#define KEY_CTRL_SHIFT_RIGHT  566
#define KEY_SHIFT_UP          337
#define KEY_SHIFT_DOWN        336
#define KEY_CTRL_DC           524
#define KEY_CTRL_BACKSPACE      8

/**
 * The document editor has a title bar at the top and a status bar at the bottom.
 * To find out whether a terminal cursor coordinate received by a mouse click
 * is inbetween those bars, so within the actual editor window,
 * the is_within_editor_bounds(y, x) function can be used.
 * It ensures that a cursor position is not in the vicinity of one of
 * either status bars.
 */
static bool is_within_editor_bounds(int y, int x) {
	return (
		x >= editor.x and x < editor.width and
		y >= editor.y and y < editor.height
	);
}

static void handle_mouse_event(void) {
	MEVENT event;
	if (getmouse(&event) == OK) {
		if (event.bstate & BUTTON1_PRESSED and is_within_editor_bounds(event.y, event.x)) {
			update_cursor_reverse(event.y, event.x);
			begin_selection();
		}
		else if (event.bstate & BUTTON1_RELEASED and is_within_editor_bounds(event.y, event.x)) {
			update_cursor_reverse(event.y, event.x);
			update_selection();
		}
		if (event.bstate & BUTTON4_PRESSED) {  /* scrollwheel down */
			if (can_scroll(-1)) {
				scroll_page(-1);
				move_up();
			}
		} else if (event.bstate & BUTTON5_PRESSED) {  /* scrollwheel up */
			if (can_scroll(1)) {
				scroll_page(1);
				move_down();
			}
		}
	}
}

static void handle_resize_event(void) {
	update_terminal_dimensions();
	clear();
	print_title_bar();
	print_page();
}

bool handle_input(int key) {
	switch (key) {
		default:
			invalidate_selection();
			insert_character_at_current_position(key);
			move_right();
			print_current_line();
			break;
		case KEY_RESIZE:
			handle_resize_event();
			break;
		case KEY_MOUSE:
			handle_mouse_event();
			break;
		case KEY_HOME:
			invalidate_selection();
			move_to_beginning_of_line();
			break;
		case KEY_END:
			invalidate_selection();
			move_to_end_of_line();
			break;
		case KEY_NPAGE:
			invalidate_selection();
			move_to_next_page();
			break;
		case KEY_PPAGE:
			invalidate_selection();
			move_to_previous_page();
			break;
		case KEY_LEFT:
			invalidate_selection();
			if (can_move_left()) {
				move_left();
			} else if (can_move_up()) {
				move_up();
				move_to_end_of_line();
			}
			break;
		case KEY_RIGHT:
			invalidate_selection();
			if (can_move_right()) {
				move_right();
			} else if (can_move_down()) {
				move_down();
				move_to_beginning_of_line();
			}
			break;
		case KEY_UP:
			invalidate_selection();
			if (can_move_up()) {
				move_up();
			}
			break;
		case KEY_DOWN:
			invalidate_selection();
			if (can_move_down()) {
				move_down();
			}
			break;
		case KEY_SLEFT:
			if (not active_selection()) {
				begin_selection();
			}
			if (can_move_left()) {
				move_left();
			} else if (can_move_up()) {
				move_up();
				move_to_end_of_line();
			}
			update_selection();
			break;
		case KEY_SRIGHT:
			if (not active_selection()) {
				begin_selection();
			}
			if (can_move_right()) {
				move_right();
			} else if (can_move_down()) {
				move_down();
				move_to_beginning_of_line();
			}
			update_selection();
			break;
		case KEY_SHIFT_UP:
			if (not active_selection()) {
				begin_selection();
			}
			if (can_move_up()) {
				move_up();
			}
			update_selection();
			break;
		case KEY_SHIFT_DOWN:
			if (not active_selection()) {
				begin_selection();
			}
			if (can_move_down()) {
				move_down();
			}
			update_selection();
			break;
		case KEY_CTRL_LEFT:
			invalidate_selection();
			seek_left();
			break;
		case KEY_CTRL_RIGHT:
			invalidate_selection();
			seek_right();
			break;
		case KEY_CTRL_SHIFT_LEFT:
			if (not active_selection()) {
				begin_selection();
			}
			seek_left();
			update_selection();
			break;
		case KEY_CTRL_SHIFT_RIGHT:
			if (not active_selection()) {
				begin_selection();
			}
			seek_right();
			update_selection();
			break;
		case KEY_BACKSPACE:
			invalidate_selection();
			if (can_move_left()) {
				move_left();
				delete_character_at_current_position();
				print_current_line();
			} else if (can_move_up()) {
				move_up();
				move_to_end_of_line();
				merge_with_next_line();
				print_page();
			}
			break;
		case KEY_DC:
			invalidate_selection();
			if (can_move_right()) {
				delete_character_at_current_position();
				print_current_line();
			} else if (can_move_down()) {
				merge_with_next_line();
				print_page();
			}
			break;
		case KEY_CTRL_BACKSPACE:
			invalidate_selection();
			if (can_move_left()) {
				move_left();
				delete_character_at_current_position();
			}
			while (can_move_left() and isalnum(current_character(-1))) {
				move_left();
				delete_character_at_current_position();
			}
			print_current_line();
			break;
		case KEY_CTRL_DC:
			invalidate_selection();
			if (can_move_right()) {
				delete_character_at_current_position();
			}
			while (can_move_right() and isalnum(current_character(0))) {
				delete_character_at_current_position();
			}
			print_current_line();
			break;
		case CTRL('q'):  /* quit */
			return false;
		case CTRL('a'):
			select_everything();
			break;
		case CTRL('c'):  /* copy selection */
			copy_current_selection();
			break;
		case CTRL('v'):  /* paste clipboard */
			paste_clipboard();
			break;
		case CTRL('x'):  /* cut selection */
			cut_current_selection();
			break;
		case CTRL('g'):  /* goto line */
			editor.line = launch_goto_line_dialog();
			editor.column = 1;
			editor.line_offset = min(normalize(editor.line) - editor.height/2, 0);
			update_current_cursor();
			print_page();
			break;
		case CTRL('s'):  /* save document */
			save_document(editor.document);
			break;
		case CTRL('f'):  /* find text */
			launch_find_text_dialog();
			break;
		case CTRL('r'):  /* replace text */
			launch_replace_text_dialog();
			break;
		case '\n':
			invalidate_selection();
			insert_line_at_current_position();
			move_down();
			move_to_beginning_of_line();
			print_page();
			break;
	}
	return true;
}