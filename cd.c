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
#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <unistd.h>

static void cd_in_cdpath(size_t n, char p[])
{
	if (p[0] == '/') {
		return;
	}

	if (p[0] == '.' && (p[1] == '\0' || p[1] == '/')) {
		return;
	}

	if (p[0] == '.' && p[1] == '.' && (p[2] == '\0' || p[2] == '/')) {
		return;
	}

	char *found = sh_find_in_path(p, "CDPATH");
	if (found != NULL) {
		snprintf(p, n, "%s", found);
		free(found);
	}
}

static void cd_make_canonical(char *path)
{
	char tmp[strlen(path) + 1];
	strcpy(tmp, path);

	/* TODO */

	strcpy(path, tmp);
}

static int cd_get_home(size_t n, char path[])
{
	char *home = getenv("HOME");

	if (home == NULL || home[0] == '\0') {
		/* implementation-defined behavior */
		/* TODO: try getpwuid() home directory */
		fprintf(stderr, "cd: missing $HOME\n");
		return 1;
	}

	snprintf(path, n, "%s", home);
	return 0;
}

int cd_main(int argc, char *argv[])
{
	enum { LOGICALLY, PHYSICALLY } dotdot = LOGICALLY;
	char curpath[PATH_MAX];
	char oldpath[PATH_MAX];

	int c;
	while ((c = getopt(argc, argv, "LP")) != -1) {
		switch (c) {
		case 'L':
			dotdot = LOGICALLY;
			break;

		case 'P':
			dotdot = PHYSICALLY;
			break;

		default:
			return 1;
		}
	}

	if (optind < argc - 1) {
		fprintf(stderr, "cd: too many arguments\n");
		return 1;
	}

	getcwd(oldpath, sizeof(oldpath));

	if (argv[optind] == NULL) {
		if (cd_get_home(sizeof(curpath), curpath) != 0) {
			return 1;
		}
	} else {
		strcpy(curpath, argv[optind]);
	}

	cd_in_cdpath(sizeof(curpath), curpath);

	if (dotdot == LOGICALLY) {
		cd_make_canonical(curpath);
	}

	if (strcmp(curpath, "-") == 0) {
		char *oldpwd = getenv("OLDPWD");
		if (oldpwd == NULL) {
			fprintf(stderr, "cd: no $OLDPWD\n");
			return 1;
		}
		if (chdir(oldpwd) != 0) {
			fprintf(stderr, "cd: %s: %s\n", oldpwd, strerror(errno));
			return 1;
		}
		puts(oldpwd);
		strcpy(curpath, oldpwd);
	} else if (chdir(curpath) != 0) {
		fprintf(stderr, "cd: %s: %s\n", curpath, strerror(errno));
		return 1;
	}

	setenv("OLDPWD", oldpath, 1);
	setenv("PWD", curpath, 1);
	return 0;
}
