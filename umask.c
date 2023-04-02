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
#include <sys/stat.h>
#include <unistd.h>

int umask_main(int argc, char *argv[])
{
	int symbolic = 0;

	int c;
	while ((c = getopt(argc, argv, "S")) != -1) {
		switch (c) {
		case 'S':	/** produce symbolic output **/
			symbolic = 1;
			break;

		default:
			return 1;
		}
	}

	if (argv[optind]) {
		fprintf(stderr, "umask: TODO\n");
		return 1;
	}

	mode_t mask = umask(0);
	umask(mask);

	if (symbolic) {
		printf("u=");
		printf("%s", mask & S_IRUSR ? "" : "r");
		printf("%s", mask & S_IWUSR ? "" : "w");
		printf("%s", mask & S_IXUSR ? "" : "x");

		printf(",g=");
		printf("%s", mask & S_IRGRP ? "" : "r");
		printf("%s", mask & S_IWGRP ? "" : "w");
		printf("%s", mask & S_IXGRP ? "" : "x");

		printf("%s", ",o=");
		printf("%s", mask & S_IRGRP ? "" : "r");
		printf("%s", mask & S_IWGRP ? "" : "w");
		printf("%s\n", mask & S_IXGRP ? "" : "x");
	} else {
		printf("%04o\n", mask);
	}

	return 0;
}
