#include <unistd.h>
#include <stdio.h>

int main(int argc, char *argv[])
{
	char *p;
	char buf[FILENAME_MAX];
	int ret;

	printf("%d\n", isatty(STDIN_FILENO));

	p = ttyname(STDIN_FILENO);
	printf("%s\n", p != NULL ? p : "<null>");

	ret = ttyname_r(STDIN_FILENO, buf, sizeof(buf));
	printf("%s\n", ret == 0 ? buf: "<null>");
	return 0;
}
