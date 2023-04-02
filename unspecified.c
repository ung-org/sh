#include <stdio.h>

int unspecified_main(int argc, char *argv[])
{
	(void)argc;
	fprintf(stderr, "sh: %s is an extension allowed by POSIX but not implemented in this shell\n", argv[0]);
	return 1;
}
