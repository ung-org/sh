#define _POSIX_C_SOURCE 200809L
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include "shed.h"

/*
 * BIG FAT TODO: DEAL WITH MULTIBYTE CHARACTERS CORRECTLY
 */

struct shed *shed(struct shed *e)
{
	if (e == NULL) {
		e = calloc(1, sizeof(*e));
		if (e == NULL) {
			return NULL;
		}
	}

	/* TODO: head/tail/history */
	if (e->cur == NULL) {
		e->cur = calloc(1, sizeof(*e->cur));
		if (e->cur == NULL) {
			return e;
		}
		e->handle = shed_handle_non_vi;
	}

	memset(e->cur, '\0', sizeof(*e->cur));

	struct termios original_tio;
	if (tcgetattr(STDIN_FILENO, &original_tio) != 0) {
		e->cur = NULL;
		return e;
	}

	struct termios tio = original_tio;
	tio.c_iflag &= ~BRKINT;
	tio.c_lflag &= ~ECHO;
	tio.c_lflag &= ~IEXTEN;
	tio.c_lflag &= ~ICANON;
	tio.c_lflag &= ~ISIG;
	tcsetattr(STDIN_FILENO, TCSADRAIN, &tio);

	if (e->prompt == NULL) {
		e->prompt = getenv("PS1");
		if (e->prompt == NULL) {
			e->prompt = "$ ";
		}
	}
	write(STDOUT_FILENO, e->prompt, strlen(e->prompt));

	char c;
	do {
		if (read(STDIN_FILENO, &c, 1) == -1) {
			//b.nread = -1;
			/* TODO: signal failure */
			break;
		}
	} while (e->handle(e, &tio, c));

	tcsetattr(STDIN_FILENO, TCSADRAIN, &original_tio);
	return e;
}
