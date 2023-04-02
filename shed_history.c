#include <stddef.h>
#include "shed.h"

int shed_history_forward(struct shed *e)
{
	if (e->cur->next != NULL) {
		e->cur = e->cur->next;
	}
	return shed_redraw(e);
}

int shed_history_backward(struct shed *e)
{
	if (e->cur->prev != NULL) {
		e->cur = e->cur->prev;
	}
	return shed_redraw(e);
}
