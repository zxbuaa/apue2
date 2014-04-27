#include <unistd.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>

static void sig_hup(int signo)
{
	printf("SIGHUP received, pid=%d\n", getpid());
}

static void pr_ids(char *name)
{
	printf("%s: pid=%d, ppid=%d, pgrp=%d, sid=%d, tpgrp=%d\n",
		name, getpid(), getppid(), getpgrp(), getsid(0), tcgetpgrp(STDIN_FILENO));
	fflush(stdout);
} 

int main(int argc, char *argv[])
{
	pid_t pid;
	char c;

	pr_ids("parent");
	if ((pid = fork()) == -1) {
		perror("fork");
		return -1;
	} else if (pid > 0) {
		sleep(5);
		return 0;
	}

	pr_ids("child");
	signal(SIGHUP, sig_hup);
	kill(getpid(), SIGTSTP);
	pr_ids("child");

	if (read(STDIN_FILENO, &c, 1) != 1) {
		perror("read");
	}
	return 0;
}
