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
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

static int show(const char *cmd, const char *path, bool showtype)
{
	(void)cmd; (void)path; (void)showtype;
	return 0;
}

int command_main(int argc, char *argv[])
{
	char *path = getenv("PATH");
	char defaultpath[PATH_MAX];
	bool showcommand = false;
	bool showtype = false;

	int c = 0;
	while ((c = getopt(argc, argv, "pvV")) != -1) {
		switch(c) {
		case 'p':
			confstr(_CS_PATH, defaultpath, sizeof(defaultpath));
			path = defaultpath;
			break;

		case 'V':
			showtype = true;
			/* fallthru */
		case 'v':
			showcommand = true;
			break;

		default:
			return 1;
		}
	}

	if (showcommand && optind != argc - 1) {
		fprintf(stderr, "command: Options -v and -V require command and no arguments\n");
		return 1;
	}

	if (showcommand) {
		return show(argv[optind], path, showtype);
	}

	/* Run command */

	return 0;
}
