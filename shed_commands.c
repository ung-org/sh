#define _POSIX_C_SOURCE 200809L
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "shed.h"

int shed_append(struct shed *e)
{
	e->count = 1;
	shed_move_forward(e);
	return shed_insert(e);
}

int shed_append_end(struct shed *e)
{
	shed_move_end(e);
	return shed_insert(e);
}

int shed_backspace(struct shed *e)
{
	int count = e->count ? e->count : 1;
	for (int i = 0; i < count; i++) {
		shed_remove_char(e->cur, 0);
	}
	return 1;
}

int shed_cancel(struct shed *e)
{
	struct buffer *b = e->cur;
	write(STDOUT_FILENO, "^C\n", 3);
	b->nread = 0;
	return 0;
}

int shed_change(struct shed *e)
{
	shed_delete_toend(e);
	return shed_append_end(e);
}

int shed_comment(struct shed *e)
{
	shed_move_cursor(e->cur, -e->cur->pos);
	shed_insert_char(e->cur, '#');
	return 0;
}

int shed_convert_case(struct shed *e)
{
	char *b = e->cur->buf;
	size_t pos = e->cur->pos;
	int count = e->count ? e->count : 1;

	/* TODO: don't overflow */
	for (int i = 0; i < count && pos + i < (size_t)e->cur->nread; i++) {
		if (isupper(b[pos + i])) {
			shed_replace_char(e->cur, tolower(b[pos + i]));
		} else if (islower(b[pos + i])) {
			shed_replace_char(e->cur, toupper(b[pos + i]));
		} else {
			shed_replace_char(e->cur, b[pos + i]);
		}
	}
	return 1;
}

int shed_delete(struct shed *e)
{
	int count = e->count ? e->count : 1;
	for (int i = 0; i < count; i++) {
		shed_remove_char(e->cur, 1);
	}
	return 1;
}

int shed_delete_toend(struct shed *e)
{
	while (e->cur->pos < (size_t)e->cur->nread) {
		shed_remove_char(e->cur, 1);
	}
	return 1;
}

int shed_eof(struct shed *e)
{
	struct buffer *b = e->cur;
	if (b->nread == 0) {
		b->buf[0] = CTRL_D;
		b->nread = 1;
		return 0;
	}
	return 1;
}

int shed_erase(struct shed *e)
{
	struct buffer *b = e->cur;
	while (b->pos != 0) {
		shed_remove_char(b, 0);
	}
	return 1;
}

int shed_execute(struct shed *e)
{
	struct buffer *b = e->cur;
	b->pos = b->nread;
	//shed_insert_char(b, '\n');
	write(STDOUT_FILENO, "\n", 1);
	if (e->handle == shed_handle_edit) {
		e->handle = shed_handle_insert;
	}
	return 0;
}

int shed_insert(struct shed *e)
{
	e->handle = shed_handle_insert;
	return 1;
}

int shed_insert_beginning(struct shed *e)
{
	shed_move_0(e);
	return shed_insert(e);
}

int shed_redraw(struct shed *e)
{
	system("tput clear");		// TODO: pfork() and cache the output
	write(STDOUT_FILENO, e->prompt, strlen(e->prompt));
	write(STDOUT_FILENO, e->cur->buf, e->cur->nread);
	int move = e->cur->nread - e->cur->pos;
	e->cur->pos = e->cur->nread;
	shed_move_cursor(e->cur, -move);
	return 1;
}

int shed_replace(struct shed *e)
{
	e->handle = shed_handle_replace;
	return 1;
}

int shed_start_over(struct shed *e)
{
	shed_move_0(e);
	return shed_change(e);
}

int shed_worderase(struct shed *e)
{
	struct buffer *b = e->cur;
	while (b->pos != 0 && (isblank(b->buf[b->pos - 1]) || ispunct(b->buf[b->pos - 1]))) {
		shed_remove_char(b, 0);
	}
	while (b->pos != 0 && !(isspace(b->buf[b->pos - 1]) || ispunct(b->buf[b->pos - 1]))) {
		shed_remove_char(b, 0);
	}
	return 1;
}
