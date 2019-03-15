#define _XOPEN_SOURCE 700
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "jobcontrol.h"

pid_t pidfrom(const char *s)
{
	if (*s == '%') {
		if (!strcmp(s, "%%") || !strcmp(s, "%+")) {
			/* return current_job; */
		} else if (!strcmp(s, "%-")) {
			/* return previous_job; */
		}

		/* %digits => job # */
		/* %string => command begins with string */
		/* %?string => command contains string */

		return -1;
	}

	return atoi(s);
}

int jobs_main(int argc, char *argv[])
{
	printf("Sorry, %s isn't implemented yet.\n", argv[0]);
	return argc;
}
