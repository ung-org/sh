#define _POSIX_C_SOURCE 200809L
#include <ctype.h>
#include <termios.h>
#include <unistd.h>
#include "shed.h"

int shed_handle_non_vi(struct shed *e, struct termios *t, char c)
{
	if (c == t->c_cc[VEOF]) {
		if (shed_eof(e) == 0) {
			return 0;
		}
	}

	if (c == t->c_cc[VINTR]) {
		return shed_cancel(e);
	}

	if (c == t->c_cc[VEOL]) {
		return shed_move_end(e);
	}

	if (c == t->c_cc[VERASE]) {
		return shed_backspace(e);
	}

	if (c == t->c_cc[VQUIT]) {
		/* ^\ */
		return 1;
	}

	if (c == t->c_cc[VSTART]) {
		/* ^Q */
		return 1;
	}

	if (c == t->c_cc[VSTOP]) {
		/* ^S */
		return 1;
	}

	if (c == t->c_cc[VSUSP]) {
		/* ^Z */
		return 1;
	}

	if (c == '\033') {
		char esc[2];
		read(STDIN_FILENO, esc, 2);
		if (esc[1] == 'A') {
			return shed_history_backward(e);
		}
		if (esc[1] == 'B') {
			return shed_history_forward(e);
		}
		if (esc[1] == 'C') {
			return shed_move_forward(e);
		}
		if (esc[1] == 'D' && e->cur->pos > 0) {
			return shed_move_backward(e);
		}
		return 1;
	}

	if (c == '\n') {
		return shed_execute(e);
	}

	if (CTRL_A <= c && c <= CTRL_Z) {
		switch (c) {
		case CTRL_A:	return shed_move_0(e);
		case CTRL_B:	return shed_move_backward(e);
		case CTRL_C:	return shed_cancel(e);
		case CTRL_D:	return shed_delete(e);
		case CTRL_E:	return shed_move_end(e);
		case CTRL_F:	return shed_move_forward(e);
		case CTRL_G:	break; // return abort(e)?
		case CTRL_H:	return shed_backspace(e);
		case CTRL_I:	return shed_complete_wordexp(e);
		case CTRL_J:	return shed_execute(e);
		case CTRL_K:	return shed_delete_toend(e);
		case CTRL_L:	return shed_redraw(e);
		case CTRL_M:	return shed_execute(e);
		case CTRL_N:	return shed_history_forward(e);
		case CTRL_O:	// return ??? /* ??? */
		case CTRL_P:	return shed_history_backward(e);
		case CTRL_Q:	// /* TTY START */
		case CTRL_R:	// return history_backsearch(e);
		case CTRL_S:	// /* TTY STOP */
		case CTRL_T:	break; // /* ??? */
		case CTRL_U:	return shed_erase(e);
		case CTRL_V:	break; // return quote(e);
		case CTRL_W:	return shed_worderase(e);
		case CTRL_X:	// /* ??? */
		case CTRL_Y:	// /* ??? */
		case CTRL_Z:	// /* ??? */
			break;
		}
		return 1;
	}

	/* regular character */
	shed_insert_char(e->cur, c);
	return 1;
}
