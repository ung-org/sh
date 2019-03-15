#define _XOPEN_SOURCE 700
#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>

#include "jobcontrol.h"

int wait_main(int argc, char *argv[])
{
	int c;
	while ((c = getopt(argc, argv, "")) != -1) {
		return 1;
	}

	if (optind >= argc) {
		/* wait for all children */
		return 0;
	}

	int ret = 0;
	do {
		pid_t pid = pidfrom(argv[optind++]);
		pid = waitpid(pid, &ret, 0);

		if (pid == -1) {
			ret = 127;
		} else if (WIFEXITED(ret)) {
			ret = WEXITSTATUS(ret);
		} else if (WIFSIGNALED(ret)) {
			ret = 128; /* + something */
		} else {
			ret = 126;
		}
	} while (argv[optind]);

	return 0;
}
