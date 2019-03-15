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

#include "sh.h"
#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

struct simple_command {
	int argc;
	char **argv;
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
	char *l = strdup(cmdline);
	char *start = l;
	while (isspace(*start)) {
		start++;
	}

	char *end = l + strlen(l) - 1;
	while (isspace(*end) && end > start) {
		*end = '\0';
		end--;
	}

	if (end <= start) {
		free(l);
		return NULL;
	}

	struct command *cmd = calloc(1, sizeof(*cmd));
	if (cmd == NULL) {
		/* TODO: should probably crash here */
		return cmd;
	}

	cmd->type = SIMPLE;
	cmd->cmd.simple.argv = calloc(100, sizeof(char *));

	char *next = strtok(start, " ");
	do {
		cmd->cmd.simple.argv[cmd->cmd.simple.argc++] = strdup(next);
	} while ((next = strtok(NULL, " ")) != NULL);
	
	free(l);
	return cmd;
}

void sh_freecmd(struct command *command)
{
	if (command->type == SIMPLE) {
		struct simple_command *c = &(command->cmd.simple);
		for (int i = 0; c->argv[i]; i++) {
			free(c->argv[i]);
		}
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
	char *path = c->argv[0];

	if (!strchr(path, '/')) {
		if (sh_is_special_builtin(path)) {
			return sh_builtin(c->argc, c->argv);
		}

		/*
		if (sh_is_function(path)) {
			return sh_function(c->argc, c->argv);
		}
		*/

		if (sh_is_regular_builtin(path)) {
			return sh_builtin(c->argc, c->argv);
		}

		path = sh_find_in_path(path, "PATH");
	}

	if (path == NULL) {
		fprintf(stderr, "sh: %s: %s\n", c->argv[0], strerror(ENOENT));
	}

	pid_t pid = fork();
	if (pid == 0) {
		execv(path, c->argv/*, exported_environ*/);
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
