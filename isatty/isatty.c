#include <unistd.h>
#include <stdio.h>

int main(int argc, char *argv[])
{
	printf("%d\n", isatty(STDIN_FILENO));
	return 0;
}
