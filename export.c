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

char **exported_environ = NULL;
static int nexports = 0;

void sh_export(const char *env)
{
	char *value = getenv(env);
	if (value == NULL) {
		return;
	}
	size_t klen = strlen(env);

	char *keyval = calloc(klen + strlen(value) + 2, 1);
	sprintf(keyval, "%s=%s", env, value);

	for (int i = 0; i < nexports; i++) {
		if (!strncmp(exported_environ[i], env, klen)
			&& exported_environ[i][klen] == '=') {
			free(exported_environ[i]);
			exported_environ[i] = keyval;
			return;
		}
	}

	nexports++;
	exported_environ = realloc(exported_environ,
		nexports * sizeof(*exported_environ));
	exported_environ[nexports-1] = keyval;
}

int export_main(int argc, char *argv[])
{
	int print = 0;

	int c;
	while ((c = getopt(argc, argv, "p")) != EOF) {
		switch (c) {
		case 'p':	/** print exported variables **/
			print = 1;
			break;

		default:
			return 1;
		}
	}

	if (print && argv[optind] != NULL) {
		fprintf(stderr, "export: can't display and export at the same time\n");
		return 1;
	}

	if (print) {
		for (char **e = exported_environ; e && *e; e++) {
			char *env = strdup(*e);
			size_t len = strlen(env);
			if (env[len - 1] == '=') {
				env[len - 1] = '\0';
			}
			printf("export %s\n", env);
			free(env);
		}
		return 0;
	}

	do {
		char *var = argv[optind++];
		char *eq = strchr(var, '=');
		if (eq) {
			putenv(var);
			*eq = '\0';
		}
		sh_export(var);
	} while (optind < argc);

	return 0;
}
