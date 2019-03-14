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
#include <signal.h>
#include <unistd.h>

static struct {
	char *name;
	char *value;
} traps[] = {
	[0] =		{ "EXIT", 0 },
	[SIGABRT] =	{ "ABRT", 0 },
	[SIGALRM] =	{ "ALRM", 0 },
	[SIGBUS] =	{ "BUS", 0 },
	[SIGCHLD] =	{ "CHLD", 0 },
	[SIGCONT] =	{ "CONT", 0 },
	[SIGFPE] =	{ "FPE", 0 },
	[SIGHUP] =	{ "HUP", 0 },
	[SIGILL] =	{ "ILL", 0 },
	[SIGINT] =	{ "INT", 0 },
	[SIGKILL] =	{ "KILL", 0 },
	[SIGPIPE] =	{ "PIPE", 0 },
	[SIGQUIT] =	{ "QUIT", 0 },
	[SIGSEGV] =	{ "SEGV", 0 },
	[SIGSTOP] =	{ "STOP", 0 },
	[SIGTERM] =	{ "TERM", 0 },
	[SIGTSTP] =	{ "TSTP", 0 },
	[SIGTTIN] =	{ "TTIN", 0 },
	[SIGTTOU] =	{ "TTOU", 0 },
	[SIGUSR1] =	{ "USR1", 0 },
	[SIGUSR2] =	{ "USR2", 0 },
	[SIGPOLL] =	{ "POLL", 0 },
	[SIGPROF] =	{ "PROF", 0 },
	[SIGSYS] =	{ "SYS", 0 },
	[SIGTRAP] =	{ "TRAP", 0 },
	[SIGURG] =	{ "URG", 0 },
	[SIGVTALRM] =	{ "VTALRM", 0 },
	[SIGXCPU] =	{ "XCPU", 0 },
	[SIGXFSZ] =	{ "XFSZ", 0 },
};
static size_t ntraps = sizeof(traps) / sizeof(traps[0]);

static char *tnums[] = {
	[0] = "EXIT",
	[HUP] = "HUP",
	[INT] = "INT",
	[QUIT] = "QUIT",
	[ABRT] = "ABRT",
	[KILL] = "KILL",
	[ALRM] = "ALRM",
	[TERM] = "TERM",
};
static size_t nnums = sizeof(tnums) / sizeof(tnums[0]);

static int trap_num(const char *name)
{
	if (!strcmp(name, "0")) {
		return 0;
	}

	int t = atoi(name);
	if (t != 0) {
		if ((size_t)t < nnums && tnums[t] != NULL) {
			name = tnums[t];
		}
	}

	for (size_t i = 0; i < ntraps; i++) {
		if (traps[i].name && !strcmp(traps[i].name, name)) {
			return i;
		}
	}
	fprintf(stderr, "trap: unknown trap '%s'\n", name);
	return -1;
}

static int trap_reset(const char *trap)
{
	int t = trap_num(trap);
	if (t == -1) {
		return 1;
	}

	free(traps[t].value);
	traps[t].value = NULL;
	signal(t, SIG_DFL);
	return 0;
}

void sh_trap(int trapno)
{
	if (traps[trapno].value) {
		/* sh_eval(traps[trapno].value); */
	}
}

static int trap_set(const char *trap, const char *action)
{
	int t = trap_num(trap);
	if (t == -1) {
		return 1;
	}

	if (t == SIGKILL || t == SIGSTOP) {
		fprintf(stderr, "trap: setting trap for %s is undefined\n", trap);
		return 1;
	}

	if (traps[t].value) {
		free(traps[t].value);
	}
	traps[t].value = strdup(action);

	if (!strcmp(action, "")) {
		signal(t, SIG_IGN);
	} else {
		signal(t, sh_trap);
	}

	return 0;
}

int trap_main(int argc, char *argv[])
{
	while (getopt(argc, argv, "") != -1) {
		return 1;
	}

	if (argv[optind] == NULL) {
		for (size_t i = 0; i < ntraps; i++) {
			if (traps[i].value) {
				printf("trap -- %s %s\n", traps[i].value, traps[i].name);
			}
		}
		return 0;
	}

	char *action = argv[optind];
	int r = 0;

	/* FIXME */
	if (atoi(action) > 0) {
		do {
			r |= trap_reset(argv[optind++]);
		} while (argv[optind]);
		return r;
	}

	optind++;

	if (!strcmp(action, "-")) {
		do {
			r |= trap_reset(argv[optind++]);
		} while (argv[optind]);
		return r;
	}

	do {
		r |= trap_set(argv[optind++], action);
	} while (argv[optind]);

	return r;
}
