#include "sh.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int type_main(int argc, char **argv)
{
	int c;
	while ((c = getopt(argc, argv, "")) != -1) {
		switch (c) {
		default:
			return 1;
		}
	}

	if (optind >= argc) {
		fprintf(stderr, "type: missing operands\n");
		return 1;
	}

	int ret = 0;
	for (int i = optind; i < argc; i++) {
		if (sh_is_special_builtin(argv[i])) {
			printf("%s: special built-in\n", argv[i]);
			continue;
		}

		/*
		if (sh_is_function(argv[i])) {
			printf("%s: function\n", argv[i]);
			continue;
		}
		*/

		if (sh_is_regular_builtin(argv[i])) {
			printf("%s: built-in\n", argv[i]);
			continue;
		}

		char *alias = sh_get_alias(argv[i]);
		if (alias != NULL) {
			printf("%s: alias (%s)\n", argv[i], alias);
			continue;
		}

		// keyword

		char *path = sh_find_in_path(argv[i], "PATH");
		if (path) {
			printf("%s: %s\n", argv[i], path);
			free(path);
			continue;
		}

		fprintf(stderr, "type: %s not found\n", argv[i]);
		ret = 1;
	}

	return ret;
}
