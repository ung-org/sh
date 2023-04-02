#define _POSIX_C_SOURCE 200809L
#include <termios.h>
#include "shed.h"

static int shed_insert_special(struct shed *e, struct termios *t, char c)
{
	if (c == '\n') {
		return shed_execute(e);
	}

	if (c == t->c_cc[VERASE]) {
		return shed_backspace(e);
	}

	if (c == t->c_cc[VINTR]) {
		return shed_cancel(e);
	}
	
	if (c == t->c_cc[VKILL]) {
		return shed_erase(e);
	}

	if (c == CTRL_V) {
		/* quote the next character */
		return 1;
	}

	if (c == CTRL_W) {
		return shed_worderase(e);
	}

	if (c == t->c_cc[VEOF]) {
		return shed_eof(e);
	}

	if (c == ESCAPE) {
		e->handle = shed_handle_edit;
		return 1;
	}

	return -1;
}

int shed_handle_insert(struct shed *e, struct termios *t, char c)
{
	int r = shed_insert_special(e, t, c);
	if (r == -1) {
		shed_insert_char(e->cur, c);
		r = 1;
	}
	return r;
}

int shed_handle_replace(struct shed *e, struct termios *t, char c)
{
	int r = shed_insert_special(e, t, c);
	if (r == -1) {
		shed_replace_char(e->cur, c);
		r = 1;
	}
	return r;
}
