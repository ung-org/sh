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
#include <string.h>
#include <unistd.h>

int exec_main(int argc, char *argv[])
{
	while (getopt(argc, argv, "") != -1) {
		return 1;
	}

	if (argv[optind] == NULL) {
		/* keep redirects for this shell */
		return 0;
	}

	char *path = sh_find_in_path(argv[optind], "PATH");
	if (path == NULL) {
		fprintf(stderr, "exec: %s: %s\n", argv[optind], strerror(ENOENT));
		exit(127);
	}

	execve(path, argv + optind, exported_environ);
	fprintf(stderr, "exec: %s: %s\n", argv[optind], strerror(errno));
	exit(126);
	return 126;
}
