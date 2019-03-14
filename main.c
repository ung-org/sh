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
#include <libgen.h>
#include <unistd.h>

int sh_main(int argc, char *argv[])
{
	enum { STDIN, INTERACTIVE, COMMAND } mode = STDIN;
	int c;
	while ((c = getopt(argc, argv, "abCefhimnuvxo:cs")) != -1) {
		switch (c) {
		case 'a':	/** export all variables **/
		case 'b':	/** notify of background job completions **/
		case 'C':	/** don't clobber existing files with > **/
		case 'e':	/** exit on failure **/
		case 'f':	/** disable pathname expansion **/
		case 'h':	/** hash utilities in functions when defined **/
		case 'n':	/** read commands but don't execute **/
		case 'u':	/** don't expand unset parameters **/
		case 'v':	/** echo input to /stderr/ **/
		case 'x':	/** enable tracing **/
			sh_set(c, 1);
			break;

		case 'o':	/** enable [option] **/
			sh_seto(optarg, 1);
			break;

		case 'c':	/** execute [command_string] and return **/
			mode = COMMAND;
			break;

		case 'i':	/** run an interactive shell **/
			mode = INTERACTIVE;
			break;

		case 's':	/** read commands from /stdin/ **/
			mode = STDIN;
			break;

		default:
			return 1;
		}
	}

	while (argv[optind] && argv[optind][0] == '+') {
		/* consume these */
		optind++;
	}

	if (mode == COMMAND) {
		/* shift argv */
		execvp(argv[optind], argv + optind);
		fprintf(stderr, "sh: %s: %s\n", argv[optind], strerror(errno));
		return 1;
	}

	if (argv[optind] == NULL && isatty(STDIN_FILENO) && isatty(STDERR_FILENO)) {
		mode = INTERACTIVE;
	}

	sh_init(mode == INTERACTIVE);

	/* set $1-$# */

	if (mode == INTERACTIVE) {
		return sh_interactive();
	}

	/* mode is STDIN */
	return yyparse();
}

int main(int argc, char *argv[])
{
	putenv("PS1=$ ");
	char *base = basename(argv[0]);
	if (sh_is_regular_builtin(base)) {
		return sh_builtin(argc, argv);
	}
	
	return sh_main(argc, argv);
}
