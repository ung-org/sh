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
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>

struct {
	char *name;
	int num;
} ksigs[] = {
	{ "ABRT",	SIGABRT },
	{ "ALRM",	SIGALRM },
	{ "BUS",	SIGBUS },
	{ "CHLD",	SIGCHLD },
	{ "CONT",	SIGCONT },
	{ "FPE",	SIGFPE },
	{ "HUP",	SIGHUP },
	{ "ILL",	SIGILL },
	{ "INT",	SIGINT },
	{ "KILL",	SIGKILL },
	{ "PIPE",	SIGPIPE },
	{ "QUIT",	SIGQUIT },
	{ "SEGV",	SIGSEGV },
	{ "STOP",	SIGSTOP },
	{ "TERM",	SIGTERM },
	{ "TSTP",	SIGTSTP },
	{ "TTIN",	SIGTTIN },
	{ "TTOU",	SIGTTOU },
	{ "USR1",	SIGUSR1 },
	{ "USR2",	SIGUSR2 },
	{ "POLL",	SIGPOLL },
	{ "PROF",	SIGPROF },
	{ "SYS",	SIGSYS },
	{ "TRAP",	SIGTRAP },
	{ "URG",	SIGURG },
	{ "VTALRM",	SIGVTALRM },
	{ "XCPU",	SIGXCPU },
	{ "XFSZ",	SIGXFSZ },
};
size_t nsigs = sizeof(ksigs) / sizeof(ksigs[0]);

int kill_main(int argc, char *argv[])
{
	char *signame = "TERM";
	int list = 0;

	opterr = 0;
	int c;
	while ((c = getopt(argc, argv, "s:l")) != -1) {
		switch (c) {
		case 's':	/** send signal [signal_name] **/
			signame = optarg;
			break;

		case 'l':
			list = 1;
			break;

		default:
			if (isdigit(optopt) || isupper(optopt)) {
				signame = argv[optind];
				break;
			}
			fprintf(stderr, "kill: unknown option '-%c'\n", optopt);
			//return 1;
		}
	}

	if (list) {
		if (argv[optind]) {
		}

		for (size_t i = 0; i < nsigs; i++) {
			printf("%s%c", ksigs[i].name, i < nsigs - 1 ? ' ' : '\n');
		}
		return 0;
	}

	if (argc <= optind) {
		fprintf(stderr, "kill: missing operand\n");
		return 1;
	}

	int pid = atoi(argv[optind]);
	for (size_t i = 0; i < nsigs; i++) {
		if (!strcmp(signame, ksigs[i].name)) {
			if (kill(pid, ksigs[i].num) != 0) {
				perror("kill");
			}
		}
	}

	fprintf(stderr, "kill: invalid signal '%s'\n", signame);
	return 1;
}
