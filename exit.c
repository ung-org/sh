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
#include <unistd.h>

int exit_main(int argc, char *argv[])
{
	static int exiting = 0;

	while (getopt(argc, argv, "") != -1) {
		return 1;
	}

	if (optind < argc - 1) {
		fprintf(stderr, "exit: only one exit status supported\n");
		return 1;
	}

	int r = 0;

	if (argv[optind]) {
		r = atoi(argv[optind]);
	}

	if (exiting == 0) {
		exiting = 1;
		sh_trap(0);
	}

	exit(r);
	_exit(r);
	return r;
}
