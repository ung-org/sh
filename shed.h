#ifndef SHED_H
#define SHED_H

#include <sys/types.h>

enum {
	CTRL_A = 0x01,
	CTRL_B = 0x02,
	CTRL_C = 0x03,
	CTRL_D = 0x04,
	CTRL_E = 0x05,
	CTRL_F = 0x06,
	CTRL_G = 0x07,
	CTRL_H = 0x08,
	CTRL_I = 0x09,
	CTRL_J = 0x0a,
	CTRL_K = 0x0b,
	CTRL_L = 0x0c,
	CTRL_M = 0x0d,
	CTRL_N = 0x0e,
	CTRL_O = 0x0f,
	CTRL_P = 0x10,
	CTRL_Q = 0x11,
	CTRL_R = 0x12,
	CTRL_S = 0x13,
	CTRL_T = 0x14,
	CTRL_U = 0x15,
	CTRL_V = 0x16,
	CTRL_W = 0x17,
	CTRL_X = 0x18,
	CTRL_Y = 0x19,
	CTRL_Z = 0x1a,
	ESCAPE = 0x1b,
};

struct termios;

struct buffer {
	struct buffer *next;
	struct buffer *prev;
	size_t len;
	size_t pos;
	ssize_t nread;
	char buf[512];	/* TODO: dynamic */
};

struct shed {
	char *prompt;
	int (*handle)(struct shed *e, struct termios *, char);
	struct buffer *head;
	struct buffer *tail;
	struct buffer *cur;
	int count;
};

void shed_replace_char(struct buffer *b, char c);
void shed_insert_char(struct buffer *b, char c);
void shed_remove_char(struct buffer *b, int forward);
void shed_move_cursor(struct buffer *b, int move);

int shed_append(struct shed *e);
int shed_append_end(struct shed *e);
int shed_backspace(struct shed *e);
int shed_cancel(struct shed *e);
int shed_change(struct shed *e);
int shed_comment(struct shed *e);
int shed_convert_case(struct shed *e);
int shed_eof(struct shed *e);
int shed_delete(struct shed *e);
int shed_delete_toend(struct shed *e);
int shed_erase(struct shed *e);
int shed_execute(struct shed *e);
int shed_insert(struct shed *e);
int shed_insert_beginning(struct shed *e);
int shed_redraw(struct shed *e);
int shed_replace(struct shed *e);
int shed_start_over(struct shed *e);
int shed_worderase(struct shed *e);

int shed_complete_wordexp(struct shed *e);

int shed_history_backward(struct shed *e);
int shed_history_forward(struct shed *e);

int shed_move_forward(struct shed *e);
int shed_move_backward(struct shed *e);
int shed_move_beginning(struct shed *e);
int shed_move_end(struct shed *e);
int shed_move_0(struct shed *e);
int shed_move_column(struct shed *e);

int shed_handle_insert(struct shed *e, struct termios *t, char c);
int shed_handle_replace(struct shed *e, struct termios *t, char c);
int shed_handle_edit(struct shed *e, struct termios *t, char c);
int shed_handle_non_vi(struct shed *e, struct termios *t, char c);

struct shed *shed(struct shed *e);

#endif
