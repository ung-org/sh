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
#include <sys/resource.h>
#include <unistd.h>

#define BLOCKSIZE (512)

int ulimit_main(int argc, char *argv[])
{
	int c;
	while ((c = getopt(argc, argv, "f")) != -1) {
		switch (c) {
		case 'f':	/** set or report limit in blocks **/
			/* is default */
			break;

		default:
			return 1;
		}
	}

	if (argv[optind]) {
		struct rlimit rl;
		rl.rlim_cur = atoi(argv[optind]) * BLOCKSIZE;
		if (setrlimit(RLIMIT_FSIZE, &rl) == -1) {
			perror("ulimit");
			return 1;
		}
		return 0;
	}

	struct rlimit rl;
	getrlimit(RLIMIT_FSIZE, &rl);
	if (rl.rlim_cur == RLIM_INFINITY) {
		printf("unlimited\n");
	} else {
		printf("%ld\n", rl.rlim_cur / BLOCKSIZE);
	}

	return 0;
}
