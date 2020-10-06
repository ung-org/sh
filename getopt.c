#define _POSIX_C_SOURCE 2
#include <sys/types.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>

int sh_getopt(int argc, char * const argv[], const char *optstring)
{
	static int optchar = 0;
	char *option = NULL;

	if (optind == 0) {
		optind = 1;
		optchar = 0;
	}

	if (argv[optind] == NULL) {
		return -1;
	}

	if (argv[optind][0] != '-') {
		return -1;
	}

	if (!strcmp(argv[optind], "-")) {
		return -1;
	}

	if (!strcmp(argv[optind], "--")) {
		optind++;
		optchar = 0;
		return -1;
	}

	if (optchar == 0 && argv[optind][optchar] != '-') {
		return -1;
	}

	optchar++;

	if (argv[optind][optchar] == '\0') {
		optind++;
		optchar = 0;
		return getopt(argc, argv, optstring);
	}

	option = strchr(optstring, argv[optind][optchar]);

	if (!option) {
		optopt = argv[optind][optchar];
		if (opterr != 0 && optstring[0] != ':') {
			fprintf(stderr, "%s: invalid option -%c\n", argv[0],
			optopt);
		}
		return '?';
	}

	if (option[1] != ':') {
		return *option;
	}

	if (argv[optind][optchar + 1] == '\0') {
		optarg = argv[++optind];
	} else {
		optarg = argv[optind] + optchar + 1;
	}

	optind++;
	if (optarg == NULL) {
		optopt = *option;
		if (opterr != 0 && optstring[0] != ':') {
			fprintf(stderr, "%s: missing argument to option -%c\n",
			argv[0], optopt);
		}
		return optstring[0] == ':' ? ':' : '?';
	}

	optchar = 0;
	return *option;
}

/*
POSIX(2)
*/
