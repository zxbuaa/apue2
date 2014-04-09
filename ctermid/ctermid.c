#include <stdio.h>

int main(int argc, char *argv[])
{
	printf("%d: %s\n", L_ctermid, ctermid(NULL));
	return 0;
}
