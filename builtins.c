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
#include <stddef.h>
#include <string.h>
#include <libgen.h>
#include <unistd.h>

struct builtin {
        const char *name;
        int (*main)(int argc, char *argv[]);
};

/* Special built-ins */
/* break */
/* : */
/* continue */
int dot_main(int argc, char *argv[]);
int eval_main(int argc, char *argv[]);
int exec_main(int argc, char *argv[]);
int exit_main(int argc, char *argv[]);
int export_main(int argc, char *argv[]);
int readonly_main(int argc, char *argv[]);
/* return */
int set_main(int argc, char *argv[]);
int shift_main(int argc, char *argv[]);
int times_main(int argc, char *argv[]);
int trap_main(int argc, char *argv[]);
int unset_main(int argc, char *argv[]);

static struct builtin special_builtins[] = {
	{ "break", NULL },
	{ ":", NULL },
	{ "continue", NULL },
	{ ".", dot_main },
	{ "eval", eval_main },
	{ "exec", exec_main },
	{ "exit", exit_main },
	{ "export", export_main },
	{ "readonly", readonly_main },
	{ "return", NULL },
	{ "set", set_main },
	{ "shift", shift_main },
	{ "times", times_main },
	{ "trap", trap_main },
	{ "unset", unset_main },
	{ 0, 0 },
};

/* Regular built-ins */
int alias_main(int argc, char *argv[]);
int bg_main(int argc, char *argv[]);
int cd_main(int argc, char *argv[]);
int command_main(int argc, char *argv[]);
int false_main(int argc, char *argv[]);
int fc_main(int argc, char *argv[]);
int fg_main(int argc, char *argv[]);
int getopts_main(int argc, char *argv[]);
int jobs_main(int argc, char *argv[]);
int kill_main(int argc, char *argv[]);
int newgrp_main(int argc, char *argv[]);
int pwd_main(int argc, char *argv[]);
int read_main(int argc, char *argv[]);
int true_main(int argc, char *argv[]);
int type_main(int argc, char *argv[]);
int ulimit_main(int argc, char *argv[]);
int umask_main(int argc, char *argv[]);
int unalias_main(int argc, char *argv[]);
int wait_main(int argc, char *argv[]);

int unspecified_main(int argc, char *argv[]);

#ifndef EXTRA_BUILTINS
#define EXTRA_BUILTINS /* empty */
#endif

static struct builtin regular_builtins[] = {
	{ "alias", alias_main },
	{ "bg", bg_main },
	{ "cd", cd_main },
	{ "command", command_main },
	{ "false", false_main },
	{ "fc", fc_main },
	{ "fg", fg_main },
	{ "getopts", getopts_main },
	{ "jobs", jobs_main },
	{ "kill", kill_main },
	{ "newgrp", newgrp_main },
	{ "pwd", pwd_main },
	{ "read", read_main },
	{ "true", true_main },
	{ "type", type_main },
	{ "ulimit", ulimit_main },
	{ "umask", umask_main },
	{ "unalias", unalias_main },
	{ "wait", wait_main },
	EXTRA_BUILTINS
	{ 0, 0 },
};

static struct builtin unspecified_builtins[] = {
	{ "alloc", unspecified_main },
	{ "autoload", unspecified_main },
	{ "bind", unspecified_main },
	{ "bindkey", unspecified_main },
	{ "builtin", unspecified_main },
	{ "bye", unspecified_main },
	{ "caller", unspecified_main },
	{ "cap", unspecified_main },
	{ "chdir", unspecified_main },
	{ "clone", unspecified_main },
	{ "copmarguments", unspecified_main },
	{ "compcall", unspecified_main },
	{ "compctl", unspecified_main },
	{ "compdescribe", unspecified_main },
	{ "compfiles", unspecified_main },
	{ "compgen", unspecified_main },
	{ "compgroups", unspecified_main },
	{ "complete", unspecified_main },
	{ "compquote", unspecified_main },
	{ "comptags", unspecified_main },
	{ "comptry", unspecified_main },
	{ "compvalues", unspecified_main },
	{ "declare", unspecified_main },
	{ "dirs", unspecified_main },
	{ "disable", unspecified_main },
	{ "disown", unspecified_main },
	{ "dosh", unspecified_main },
	{ "echotc", unspecified_main },
	{ "echoti", unspecified_main },
	{ "help", unspecified_main },
	{ "history", unspecified_main },
	{ "hist", unspecified_main },
	{ "let", unspecified_main },
	{ "local", unspecified_main },
	{ "login", unspecified_main },
	{ "logout", unspecified_main },
	{ "map", unspecified_main },
	{ "mapfile", unspecified_main },
	{ "popd", unspecified_main },
	{ "print", unspecified_main },
	{ "pushed", unspecified_main },
	{ "readarray", unspecified_main },
	{ "repeat", unspecified_main },
	{ "savehistory", unspecified_main },
	{ "source", unspecified_main },
	{ "shopt", unspecified_main },
	{ "stop", unspecified_main },
	{ "suspend", unspecified_main },
	{ "typeset", unspecified_main },
	{ "whence", unspecified_main },
	{ 0, 0 },
};

static int (*sh_getbuiltin(const char *util, struct builtin *bi))(int argc, char *argv[])
{
	for (int i = 0; bi[i].name; i++) {
		if (!strcmp(bi[i].name, util)) {
			return bi[i].main;
		}
	}

	return NULL;
}

int sh_builtin(int argc, char *argv[])
{
	char *util = basename(argv[0]);
	if (!strcmp(util, "break")) {
		return BREAK;
	} else if (!strcmp(util, ":")) {
		return 0;
	} else if (!strcmp(util, "continue")) {
		return CONTINUE;
	} else if (!strcmp(util, "return")) {
		return RETURN;
	}

	int (*m)(int, char *[]) = sh_getbuiltin(util, special_builtins);
	if (m == NULL) {
		m = sh_getbuiltin(util, regular_builtins);
	}

	if (m == NULL) {
		m = sh_getbuiltin(util, unspecified_builtins);
	}

	if (m == NULL) {
		return 1;
	}
	optind = 0;

	return m(argc, argv);
}

static int is_builtin(const char *util, struct builtin *bi)
{
	for (int i = 0; bi[i].name; i++) {
		if (!strcmp(bi[i].name, util)) {
			return 1;
		}
	}

	return 0;
}

int sh_is_special_builtin(const char *util)
{
	return is_builtin(util, special_builtins);
}

int sh_is_regular_builtin(const char *util)
{
	return is_builtin(util, regular_builtins);
}

int sh_is_unspecified(const char *util)
{
	return is_builtin(util, unspecified_builtins);
}

#define main true_main
#include "true/true.c"
#undef main

#define main false_main
#include "false/false.c"
#undef main
