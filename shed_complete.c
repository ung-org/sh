#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <wordexp.h>
#include "shed.h"

static void shed_reprompt(struct shed *e)
{
	write(STDOUT_FILENO, e->prompt, strlen(e->prompt));
	size_t pos = e->cur->pos;
	e->cur->pos = 0;
	e->count = (int)pos + 1;
	shed_move_column(e);
	e->count = 0;
}

int shed_complete_wordexp(struct shed *e)
{
	char *start = e->cur->buf + e->cur->pos;
	while (!isblank(*start) && start != e->cur->buf) {
		start--;
	}
	if (isblank(*start)) {
		start++;
	}

	char *word = calloc(1, strlen(start) + 2);
	strcpy(word, start);
	char *end = word;
	while (!isblank(*end) && *end != '\0') {
		end++;
	}
	*end = '\0';

	if (strpbrk(word, "*?[") == NULL) {
		strcat(word, "*");
	}

	wordexp_t we = { 0 };
	wordexp(word, &we, WRDE_NOCMD);
	for (size_t i = 0; i < we.we_wordc; i++) {
		write(STDOUT_FILENO, i == 0 ? "\n" : "\t", 1);
		write(STDOUT_FILENO, we.we_wordv[i], strlen(we.we_wordv[i]));
	}
	write(STDOUT_FILENO, "\n", 2);
	wordfree(&we);

	free(word);
	shed_reprompt(e);
	return 1;
}
