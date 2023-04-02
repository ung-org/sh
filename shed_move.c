#define _POSIX_C_SOURCE 200809L
#include <ctype.h>
#include "shed.h"

int shed_move_forward(struct shed *e)
{
	shed_move_cursor(e->cur, e->count ? e->count : 1);
	return 1;
}

int shed_move_backward(struct shed *e)
{
	shed_move_cursor(e->cur, e->count ? -e->count : -1);
	return 1;
}

int shed_move_beginning(struct shed *e)
{
	shed_move_cursor(e->cur, -e->cur->pos);
	while (isspace(e->cur->buf[e->cur->pos])) {
		shed_move_cursor(e->cur, 1);
	}
	return 1;
}

int shed_move_end(struct shed *e)
{
	shed_move_cursor(e->cur, e->cur->nread);
	return 1;
}

int shed_move_0(struct shed *e)
{
	shed_move_cursor(e->cur, -e->cur->pos);
	return 1;
}

int shed_move_column(struct shed *e)
{
	if (e->count == 0) {
		e->count = 1;
	}
	shed_move_cursor(e->cur, e->count - e->cur->pos - 1);
	return 1;
}
