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

#include "sh.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

struct alias {
	char *alias;
	char *command;
};

static struct alias *aliases = NULL;

static void set_alias(const char *alias, const char *command)
{
	(void)alias; (void)command;
}

char * sh_get_alias(const char *alias)
{
	if (aliases == NULL) {
		return NULL;
	}

	for (int i = 0; aliases[i].alias; i++) {
		if (!strcmp(aliases[i].alias, alias)) {
			return aliases[i].command;
		}
	}

	return NULL;
}

static void show_alias(const char *alias)
{
	char *command = sh_get_alias(alias);
	if (command) {
		printf("%s=%s\n", alias, command);
	} else {
		fprintf(stderr, "alias: %s: not found\n", alias);
	}
}

int alias_main(int argc, char *argv[])
{
	while (getopt(argc, argv, "") != -1) {
		return 1;
	}

	if (argv[optind] == NULL) {
		for (int i = 0; aliases && aliases[i].alias; i++) {
			printf("%s=%s\n", aliases[i].alias, aliases[i].command);
		}
		return 0;
	}

	do {
		char *a = argv[optind++];
		char *eq = strchr(a, '=');
		if (eq) {
			*eq = '\0';
			set_alias(a, eq + 1);
		} else {
			show_alias(a);
		}
	} while (optind < argc);

	return 0;
}

int unalias_main(int argc, char **argv)
{
	printf("Sorry, %s isn't implemented yet.\n", argv[0]);
	return argc;
}
