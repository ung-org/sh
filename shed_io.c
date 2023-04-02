#define _POSIX_C_SOURCE 200809L
#include <string.h>
#include <unistd.h>
#include "shed.h"

void shed_replace_char(struct buffer *b, char c)
{
	if (b->pos >= (size_t)b->nread) {
		shed_insert_char(b, c);
		return;
	}
	b->buf[b->pos] = c;
	write(STDOUT_FILENO, &c, 1);
	b->pos++;
}

void shed_insert_char(struct buffer *b, char c)
{
	if (b->pos >= (size_t)b->nread) {
		b->buf[b->pos] = c;
		write(STDOUT_FILENO, &c, 1);
	} else {
		size_t shift = b->nread - b->pos;
		char *start = b->buf + b->pos;
		memmove(start + 1, start, shift);
		*start = c;

		char back[shift];
		memset(back, '\b', shift);
		write(STDOUT_FILENO, start, shift + 1);
		write(STDOUT_FILENO, back, shift);
	}

	b->pos++;
	b->nread++;
}

void shed_remove_char(struct buffer *b, int forward)
{
	/* TODO: don't forward delete past the end */
	if (forward) {
		write(STDOUT_FILENO, b->buf + b->pos, 1);
		b->pos++;
	}

	if (b->pos == 0) {
		return;
	}

	b->pos--;

	char *start = b->buf + b->pos;
	size_t shift = (size_t)b->nread - b->pos;

	b->nread--;

	for (size_t i = 0; i < shift; i++) {
		start[i] = start[i + 1];
	}
	start[shift] = '\0';

	char back[shift];
	memset(back, '\b', shift);
	write(STDOUT_FILENO, back, 1);
	write(STDOUT_FILENO, start, shift);
	write(STDOUT_FILENO, " \b", 2);
	write(STDOUT_FILENO, back, shift - 1);
}

void shed_move_cursor(struct buffer *b, int move)
{
	if (move > 0) {
		int maxfwd = (int)(b->nread - b->pos);
		if (move > maxfwd) {
			move = maxfwd;
		}
		write(STDOUT_FILENO, b->buf + b->pos, move);
		b->pos += move;
	}

	if (move < 0) {
		size_t len = (size_t)(-move);
		if (len > b->pos) {
			len = b->pos;
		}
		char back[len];
		memset(back, '\b', len);
		write(STDOUT_FILENO, back, len);
		b->pos -= len;
	}
}
