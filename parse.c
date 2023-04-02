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
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>
#include <wordexp.h>

struct simple_command {
	wordexp_t we;
	char **redirection;
};

struct pipeline_command {
	struct simple_command s;
};

struct list_command {
	struct simple_command s;
};

struct compound_command {
	enum { GROUPING, FOR, CASE, IF, UNTIL } type;
	struct simple_command *commands;
};

struct command {
	enum { EMPTY, SIMPLE, PIPELINE, LIST, COMPOUND } type;
	union {
		struct simple_command simple;
		struct pipeline_command pipeline;
		struct list_command list;
		struct compound_command compound;
	} cmd;
};

struct command *sh_parse(const char *cmdline)
{
	struct command *cmd = calloc(1, sizeof(*cmd));
	if (cmd == NULL) {
		return cmd;
	}
	cmd->type = SIMPLE;

	if (wordexp(cmdline, &cmd->cmd.simple.we, WRDE_SHOWERR) != 0) {
		free(cmd);
		return NULL;
	}

	char *alias = sh_get_alias(cmd->cmd.simple.we.we_wordv[0]);
	if (alias) {
		if (cmd->cmd.simple.we.we_wordc == 1) {
			free(cmd);
			return sh_parse(alias);
		}

		/* TODO: handle parameters */
	}

	return cmd;
}

void sh_freecmd(struct command *command)
{
	if (command->type == SIMPLE) {
		wordfree(&command->cmd.simple.we);
	}
	free(command);
}

char *sh_find_in_path(const char *file, const char *pathvar)
{
	char *path = getenv(pathvar);
	if (path == NULL) {
		return NULL;
	}

	path = strdup(path);
	char *checkpath = strtok(path, ":");
	do {
		char fullpath[strlen(checkpath) + strlen(file) + 2];
		sprintf(fullpath, "%s/%s", checkpath, file);
		if (access(fullpath, F_OK) == 0) {
			free(path);
			return strdup(fullpath);
		}
	} while ((checkpath = strtok(NULL, ":")) != NULL);

	free(path);
	return NULL;
}

int sh_simple_command(struct simple_command *c)
{
	char *path = c->we.we_wordv[0];

	if (!strchr(path, '/')) {
		if (sh_is_special_builtin(path)) {
			return sh_builtin(c->we.we_wordc, c->we.we_wordv);
		}

		if (sh_is_unspecified(path)) {
			return sh_builtin(c->we.we_wordc, c->we.we_wordv);
			return 1;
		}

		/*
		if (sh_is_function(path)) {
			return sh_function(c->we.we_wordc, c->we.we_wordv);
		}
		*/

		if (sh_is_regular_builtin(path)) {
			return sh_builtin(c->we.we_wordc, c->we.we_wordv);
		}

		path = sh_find_in_path(path, "PATH");
	}

	if (path == NULL) {
		fprintf(stderr, "sh: %s: %s\n", c->we.we_wordv[0], strerror(ENOENT));
		return 1;
	}

	pid_t pid = fork();
	if (pid == 0) {
		extern char **environ;
		execve(path, c->we.we_wordv, environ);
		fprintf(stderr, "sh: %s: %s\n", path, strerror(errno));
		exit(1);
	} 

	if (pid < 0) {
		perror("sh");
		return 1;
	}

	int rv;
	waitpid(pid, &rv, 0);
	return rv;
}

int sh_execute(struct command *command)
{
	if (command->type == EMPTY) {
		return 0;
	}

	if (command->type == SIMPLE) {
		return sh_simple_command(&(command->cmd.simple));
	}

	return 0;
}
