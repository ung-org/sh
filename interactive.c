/*
 * UNG's Not GNU
 *
 * Copyright (c) 2011-2017, Jakob Kaivo <jkk@ung.org>
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 */

#define _XOPEN_SOURCE 700

#include "sh.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static char *sh_getline(char **cmdline)
{
	char *prompt = getenv(*cmdline ? "PS2" : "PS1");
	/* wordexpand prompt */
	fprintf(stderr, "%s", prompt);

	size_t n = 0;
	getline(cmdline, &n, stdin);
	return *cmdline;
}

int sh_interactive(void)
{
	while (!feof(stdin)) {
		char *cmdline = NULL;
		sh_getline(&cmdline);

		if (cmdline == NULL) {
			break;
		}

		if (!strcmp(cmdline, "\n")) {
			free(cmdline);
			continue;
		}

		struct command *command = sh_parse(cmdline);
		while (command == NULL) {
			/* append more text */
			/* attempt parsing again */
		}

		sh_execute(command);

		free(cmdline);
		sh_freecmd(command);
	}

	return 0;
}
