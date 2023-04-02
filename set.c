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
#include <string.h>
#include <unistd.h>

static struct option {
	char c;
	char *s;
	int value;
} sh_options[] = {
	{ 'a', "allexport", 0 },
	{ 'b', NULL, 0 },
	{ 'C', "noclobber", 0 },
	{ 'e', "errexit", 0 },
	{ 'f', "noglob", 0 },
	{ 'h', NULL, 0 },
	{ 'm', "monitor", 0 },
	{ 'n', NULL, 0 },
	{ 'u', "nounset", 0 },
	{ 'v', "verbose", 0 },
	{ 'x', "xtrace", 0 },
	{ '-', "ignoreeof", 0 },
	{ '-', "nolog", 0 },
	{ '-', "vi", 0 },
};
static size_t noptions = sizeof(sh_options) / sizeof(sh_options[0]);

void sh_set(char option, int value)
{
	for (size_t i = 0; i < noptions; i++) {
		if (sh_options[i].c == option) {
			sh_options[i].value = value;
			return;
		}
	}
}

int sh_get(char option)
{
	for (size_t i = 0; i < noptions; i++) {
		if (sh_options[i].c == option) {
			return sh_options[i].value;
		}
	}
	return 0;
}

void sh_seto(char *option, int value)
{
	for (size_t i = 0; i < noptions; i++) {
		if (sh_options[i].s && !strcmp(sh_options[i].s, option)) {
			sh_options[i].value = value;
			return;
		}
	}
}

int sh_geto(char *option)
{
	for (size_t i = 0; i < noptions; i++) {
		if (sh_options[i].s && !strcmp(sh_options[i].s, option)) {
			return sh_options[i].value;
		}
	}
	return 0;
}

int set_main(int argc, char *argv[])
{
	int show = 0;

	int c;
	while ((c = getopt(argc, argv, ":abCefhmnuvxo:")) != -1) {
		switch (c) {
		case 'a':
		case 'b':
		case 'C':
		case 'e':
		case 'f':
		case 'h':
		case 'm':
		case 'n':
		case 'u':
		case 'v':
		case 'x':
			sh_set(c, 1);
			break;

		case 'o':
			sh_seto(optarg, 1);
			break;

		case ':':
			if (optopt != 'o') {
				return 1;
			}
			show = 1;
			break;

		default:
			return 1;
		}
	}

	if (show) {
		for (size_t i = 0; i < noptions; i++) {
			if (sh_options[i].value) {
				putchar(sh_options[i].c);
			}
		}
		putchar('\n');
	}

	return 0;
}
