#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
	pid_t pid;
	int fd;

	if ((pid = fork()) == -1) {
		perror("fork");
		return -1;
	} else if (pid > 0) {
		int status;
		if (waitpid(pid, &status, 0) == -1) {
			perror("waitpid");
			return -1;
		}
		if (WIFEXITED(status)) {
			if (WEXITSTATUS(status) != 0) {
				fprintf("child exit %d\n", WEXITSTATUS(status));
				return -1;
			}
		} else if (WIFSIGNALED(status)) {
			fprintf("child termiated by signal %d\n", WTERMSIG(status));
			return -1;
		}
		printf("parent: pid=%d, ppid=%d, pgrp=%d, sid=%d, tpgrp=%d\n",
			getpid(), getppid(), getpgrp(), getsid(0), tcgetpgrp(STDIN_FILENO));
		return 0;
	}

	printf("child: pid=%d, ppid=%d, pgrp=%d, sid=%d, tpgrp=%d\n",
		getpid(), getppid(), getpgrp(), getsid(0), tcgetpgrp(STDIN_FILENO));
	if ((fd = open("/dev/tty", O_RDONLY)) == -1) {
		perror("open controlling terminal failed before creating session");
	} else {
		printf("open controlling terminal succeed before creating session\n");
		close(fd);
	}
	setsid();
	printf("child: pid=%d, ppid=%d, pgrp=%d, sid=%d, tpgrp=%d\n",
		getpid(), getppid(), getpgrp(), getsid(0), tcgetpgrp(STDIN_FILENO));
	if ((fd = open("/dev/tty", O_RDONLY)) == -1) {
		perror("open controlling terminal failed after creating session");
	} else {
		printf("open controlling terminal succeed after creating session\n");
		close(fd);
	}

	return 0;
}

