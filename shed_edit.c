#define _POSIX_C_SOURCE 200809L
#include <ctype.h>
#include <stdlib.h>
#include "shed.h"

int shed_handle_edit(struct shed *e, struct termios *t, char c)
{
	(void)t;
	int r = -1;

	switch (c) {
	case '\n':	r = shed_execute(e);		break;
	case CTRL_L:	r = shed_redraw(e);		break;
	case '#':	r = shed_comment(e);		break;

	case '=':	// r = shed_complete_wordexp(e);	break;
	case '\\':	// r = shed_complete_pathname(e);	break;
	case '*':	// r = shed_complete_wildcard(e);	break;

	case '@':	// r = shed_insert_alias(e);	// read one letter

	case '~':	r = shed_convert_case(e);	break;
	case '.':	// r = shed_repeat(e);	break;
	case 'v':	// r = shed_invoke_vi(e);	break;

	case 'l':	/* FALLTHROUGH */
	case ' ':	r = shed_move_forward(e);	break;
	case 'h':	r = shed_move_backward(e);	break;
	case 'w':	// r = shed_move_forword(e);	break;
	case 'W':	// r = shed_move_forbigword(e);	break;
	case 'e':	// r = shed_move_endword(e);	break;
	case 'E':	// r = shed_move_endbigword(e);	break;
	case 'b':	// r = shed_move_backword(e);	break;
	case 'B':	// r = shed_move_backbigword(e);	break;
	case '^':	r = shed_move_beginning(e);	break;
	case '$':	r = shed_move_end(e);	break;
	case '0':	r = shed_move_0(e);		break;
	case '|':	r = shed_move_column(e);	break;

	case 'f':	// r = shed_move_first(e);	break;
	case 'F':	// r = shed_move_prev(e);	break;
	case 't':	// r = shed_move_before(e);	break;
	case 'T':	// r = shed_move_after(e);	break;
	case ';':	// r = shed_repeat_fftt(e);	break;
	case ',':	// r = shed_reverse_fftt(e);	break;

	case 'a':	r = shed_append(e);			break;
	case 'A':	r = shed_append_end(e);		break;
	case 'i':	r = shed_insert(e);			break;
	case 'I':	r = shed_insert_beginning(e);	break;
	case 'R':	r = shed_replace(e);			break;

	case 'c':	// r = shed_delete_to(e);	break;
	case 'C':	r = shed_change(e);		break;
	case 'S':	r = shed_start_over(e);	break;

	case 'r':	// r = shed_replace(e);	break;
	case '_':	// ???
	case 'x':	r = shed_delete(e);		break;
	case 'X':	r = shed_backspace(e);	break;
	case 'd':	// r = shed_delete_motion(e);	break;
	case 'D':	r = shed_delete_toend(e);		break;
	case 'y':	// r = shed_yank(e);		break;
	case 'Y':	// r = shed_bigyank(e);	break;
	case 'p':	// r = shed_paste(e);	break;
	case 'P':	// r = shed_bigpaste(e);	break;

	case 'u':	// r = shed_undo(e);		break;
	case 'U':	// r = shed_bigundo(e);	break;

	case 'k':	/* FALLTHROUGH */
	case '-':	// r = shed_history_back(e);	break;

	case 'j':	/* FALLTHOUGH */
	case '+':	// r = shed_history_forward(e);	break;

	case 'G':	// r = shed_history_first(e);	break;
	case '/':	// r = shed_history_backsearch(e);	break;
	case '?':	// r = shed_history_forwardsearch(e);	break;
	case 'n':	// r = shed_history_repeat(e);	break;
	case 'N':	// r = shed_history_reverse(e);	break;

	default:
		break;
	}

	if (isdigit(c)) {
		char digit[2] = { c, '\0' };
		e->count = (e->count * 10) + atoi(digit);
	}

	if (r != -1) {
		e->count = 0;
	}

	return r;
}
