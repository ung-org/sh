#include <stddef.h>
#include <string.h>
#include <unistd.h>
#include "shed.h"

static int shed_refresh(struct shed *e, struct buffer *next)
{
	shed_move_end(e);
	size_t s = e->cur->pos;
	char clrbuf[s];

	memset(clrbuf, '\b', s);
	write(STDOUT_FILENO, clrbuf, s);

	memset(clrbuf, ' ', s);
	write(STDOUT_FILENO, clrbuf, s);

	memset(clrbuf, '\b', s);
	write(STDOUT_FILENO, clrbuf, s);

	e->cur = next;
	e->cur->pos = 0;
	shed_move_end(e);
	return 1;
}

int shed_history_forward(struct shed *e)
{
	if (e->cur->next != NULL) {
		return shed_refresh(e, e->cur->next);
	}
	return 1;
}

int shed_history_backward(struct shed *e)
{
	if (e->cur->prev != NULL) {
		return shed_refresh(e, e->cur->prev);
	}
	return 1;
}
