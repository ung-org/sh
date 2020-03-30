#ifndef SH_H
#define SH_H

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
#define _POSIX_C_SOURCE 200809L

struct sh_redirect {
	int fd;
	enum { WRITE, APPEND, OVERWRITE, WRITEFD, READWRITE, READ, READFD, READHERE, READDASH } direction;
	char *name;
};

struct sh_redirect *sh_add_redirect(int fd, int direction, char *name);

struct sh_command {
	int argc;
	char *cmd;
	char **argv;
	struct sh_redirect *redirect;
};

struct command;
struct command *sh_parse(const char *cmdline);
void sh_freecmd(struct command *cmd);
int sh_execute(struct command *cmd);

void sh_set(char option, int value);
void sh_seto(char *option, int value);
int sh_get(char option);
int sh_geto(char *option);

int sh_interactive(void);

int sh_is_special_builtin(const char *util);
int sh_is_regular_builtin(const char *util);
int (*sh_get_builtin(const char *util))(int argc, char *argv[]);
int sh_builtin(int argc, char *argv[]);

enum { BREAK = -1, CONTINUE = -2, RETURN = -3 };

char *sh_find_in_path(const char *file, const char *pathvar);

void sh_export(const char *var);
extern char **exported_environ;

void sh_init(int interactive);

enum { HUP = 1, INT = 2, QUIT = 3, ABRT = 6, KILL = 9, ALRM = 14, TERM = 15 };

void sh_trap(int trapno);

int yyparse(void);

#define getopt sh_getopt

#endif
