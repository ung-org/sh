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
	struct alias *next;
	struct alias *prev;
	char *alias;
	char *command;
};

static struct alias *aliases = NULL;

static int set_alias(const char *alias, const char *command)
{
	struct alias *p = aliases;
	for (; p != NULL; p = p->next) {
		if (strcmp(p->alias, alias) == 0) {
			free(p->command);
			p->command = strdup(command);
			return 0;
		}
	}

	struct alias *a = calloc(1, sizeof(*a));
	if (a == NULL) {
		return 1;
	}

	a->alias = strdup(alias);
	a->command = strdup(command);

	if (aliases == NULL) {
		aliases = a;
		return 0;
	}

	for (p = aliases; p->next != NULL; p = p->next) {
		/* find tail */
	}
	p->next = a;
	a->prev = p;

	return 0;
}

char * sh_get_alias(const char *alias)
{
	for (struct alias *p = aliases; p != NULL; p = p->next) {
		if (!strcmp(p->alias, alias)) {
			return p->command;
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
		for (struct alias *p = aliases; p != NULL; p = p->next) {
			printf("%s=%s\n", p->alias, p->command);
		}
		return 0;
	}

	int ret = 0;
	do {
		char *a = argv[optind++];
		char *eq = strchr(a, '=');
		if (eq) {
			*eq = '\0';
			ret |= set_alias(a, eq + 1);
		} else {
			show_alias(a);
		}
	} while (optind < argc);

	return ret;
}

static struct alias *free_alias(struct alias *alias)
{
	struct alias *next = alias->next;
	if (alias == aliases) {
		aliases = next;
	}

	if (alias->next) {
		alias->next->prev = alias->prev;
	}

	if (alias->prev) {
		alias->prev->next = alias->next;
	}

	free(alias->alias);
	free(alias->command);
	free(alias);
	return next;
}

int unalias_main(int argc, char **argv)
{
	int all = 0;

	int c = 0;
	while ((c = getopt(argc, argv, "a")) != -1) {
		switch (c) {
		case 'a':
			all = 1;
			break;

		default:
			return 1;
		}
	}

	if (all) {
		if (optind < argc) {
			fprintf(stderr, "unalias: -a takes no arguments\n");
			return 1;
		}
		for (struct alias *p = aliases; p != NULL; p = free_alias(p)) {
			/* handled by update step */
		}
		return 0;
	}

	if (optind >= argc) {
		fprintf(stderr, "unalias: missing operands\n");
		return 1;
	}

	int ret = 0;
	for (int i = optind; i < argc; i++) {
		int removed = 0;
		for (struct alias *p = aliases; p != NULL; p = p->next) {
			if (strcmp(p->alias, argv[i]) == 0) {
				removed = 1;
				free_alias(p);
				break;
			}
		}
		if (!removed) {
			fprintf(stderr, "unalias: %s not defined\n", argv[i]);
			ret = 1;
		}
	}

	return ret;
}
