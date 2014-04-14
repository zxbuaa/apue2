#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <string.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>

#define MAX_PASS_LEN 8

char *my_getpass(const char *promp)
{
	int fd;
	static char buf[MAX_PASS_LEN + 1]; /* len + '\n' */
	int len;
	int rlen;
	char *bufp;
	sigset_t sig, osig;
	struct termios ts, ots;

	/* open controlling terminal */
	if ((fd = open(ctermid(NULL), O_RDWR)) == -1) {
		return NULL;
	}

	/* block SIGINT and SIGTSTP */
	/* sigemptyset(&sig);
	sigaddset(&sig, SIGINT);
	sigaddset(&sig, SIGTSTP);
	sigprocmask(SIG_BLOCK, &sig, &osig);
	*/

	/* disable echo */
	tcgetattr(fd, &ts);
	ots = ts;
	ts.c_lflag &= ~(ECHO | ECHOE | ECHOK | ECHOKE | ISIG);
	tcsetattr(fd, TCSAFLUSH, &ts);

	fprintf(stderr, "%s", promp);
	/* read a line */
	len = 0;
	while ((rlen = read(fd, buf + len, sizeof(buf) - len)) != 0) {
		len += rlen;
		if (buf[len -1 ] == '\n') {
			break;
		}
	}
	if (len == 0) {
		buf[0] = '\0';
	} else {
		if (buf[len - 1] == '\n') {
			buf[len - 1] = '\0'; /* replace '\n' with '\0' */
		}
		buf[MAX_PASS_LEN] = '\0';
	}
	bufp = buf;
	fprintf(stderr, "\n");
	tcdrain(STDERR_FILENO);

restore:
	tcsetattr(fd, TCSAFLUSH, &ots);
	/* sigprocmask(SIG_SETMASK, &osig, NULL);
	 */
	close(fd);
	return bufp;
}

char *my_getpass2(const char *promp)
{
	FILE *fp;
	int fd;
	static char buf[MAX_PASS_LEN + 2]; /* len + '\n' + '\0' */
	char *bufp;
	sigset_t sig, osig;
	struct termios ts, ots;

	/* open controlling terminal */
	if ((fp = fopen(ctermid(NULL), "r+")) == NULL) {
		return NULL;
	}
	fd = fileno(fp);
	setbuf(fp, NULL);

	/* block SIGINT and SIGTSTP */
	/* sigemptyset(&sig);
	sigaddset(&sig, SIGINT);
	sigaddset(&sig, SIGTSTP);
	sigprocmask(SIG_BLOCK, &sig, &osig);
	*/

	/* disable echo */
	tcgetattr(fd, &ts);
	ots = ts;
	ts.c_lflag &= ~(ECHO | ECHOE | ECHOK | ECHOKE | ISIG);
	tcsetattr(fd, TCSAFLUSH, &ts);

	fprintf(stderr, "%s", promp);
	/* read a line */
	if ((bufp = fgets(buf, sizeof(buf), fp)) == NULL) {
		buf[0] = '\0';
		bufp = buf;
	} else {
		int len = strlen(bufp);
		if (bufp[len - 1] == '\n') {
	   		bufp[len - 1] = '\0';
		}
		bufp[MAX_PASS_LEN] = '\0';
	}
	fprintf(stderr, "\n");

restore:
	tcsetattr(fd, TCSAFLUSH, &ots);
	/* sigprocmask(SIG_SETMASK, &osig, NULL);
	 */
	fclose(fp);
	return bufp;
}

int main(int argc, char *argv[])
{
	char *passwd;
	if ((passwd  = my_getpass("Enter 1st password: ")) != NULL) {
		printf("Your password is: %s\n", passwd);
	} else {
		perror("getpass");
	}
	if ((passwd  = my_getpass2("Enter 2nd password: ")) != NULL) {
		printf("Your password is: %s\n", passwd);
	} else {
		perror("getpass");
	}
	if ((passwd  = getpass("Enter 3nd password: ")) != NULL) {
		printf("Your password is: %s\n", passwd);
	} else {
		perror("getpass");
	}
	return 0;
}

