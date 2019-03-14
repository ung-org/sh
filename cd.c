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
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <unistd.h>

#define FLAG_L (1)
#define FLAG_P (2)

static int cd(char *path, int flag)
{
	int alloced = 0;
	int print = 0;

	if (path == NULL) {
		path = getenv("HOME");
		if (path == NULL || !strcmp(path, "")) {
			fprintf(stderr, "cd: Directory not specified and HOME not found\n");
			return 1;
		}
	}

	if (!strcmp(path, "-")) {
		path = getenv("OLDPWD");
		print = 1;
	}

	/*
	if (!(path[0] == '/' || !strncmp(path, "./", 2) || !strncmp(path, "../", 3))) {
		char *opath = path;
		path = sh_find_in_path(opath, "CDPATH");
		if (path == NULL) {
			fprintf(stderr, "cd: %s not found in CDPATH\n", opath);
			return 1;
		}
		alloced = 1;
	}
	*/

	if (flag != FLAG_P) {
		if (path[0] != '/') {
			char *base = getenv("PWD");
			char newpath[strlen(base) + strlen(path)];
			sprintf(newpath, "%s/%s", base, path);
			strcpy(path, newpath);
		}

		/* TODO: canonicalize path */
		/* TODO: shorten to <= PATH_MAX */
	}

	char *oldpwd = getenv("PWD");
	if (chdir(path) != 0) {
		if (alloced) {
			free(path);
		}

		fprintf(stderr, "cd: Couldn't change to %s: %s\n", path, strerror(errno));
		return 1;
	}

	if (alloced) {
		free(path);
	}

	if (print) {
		puts(path);
	}

	setenv("OLDPWD", oldpwd, 1);
	setenv("PWD", path, 1);
	return 0;
}

int cd_main(int argc, char *argv[])
{
	int flag = 0;
	int c;

	while ((c = getopt(argc, argv, ":LP")) != -1) {
		switch (c) {
		case 'L':
			flag = FLAG_L;
			break;

		case 'P':
			flag = FLAG_P;
			break;

		default:
			return 1;
		}
	}

	if (optind < argc - 1) {
		fprintf(stderr, "cd: too many arguments\n");
		return 1;
	}

	return cd(argv[optind], flag);
}
