#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <termios.h>
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "list.h"

struct name_list {
	char *name;
	struct list_head list;
};
LIST_HEAD(path_name);

int find_tty(const struct stat *sbuf)
{
	DIR *dir;
	struct dirent *dirent;
	int found = 0;

	if ((dir = opendir(".")) == NULL) {
		perror("fdopendir");
		return 0;
	}
	while ((dirent = readdir(dir)) != NULL) {
		struct stat sbuf_cadidate;
		if (strcmp(dirent->d_name, ".") == 0 ||
			strcmp(dirent->d_name, "..") == 0) {
			continue;
		}
		//printf("cadidate: %s\n", dirent->d_name);
		if (lstat(dirent->d_name, &sbuf_cadidate) == -1) {
			perror("stat");
			continue;
		}
		if (S_ISDIR(sbuf_cadidate.st_mode)) {
			if (chdir(dirent->d_name) == -1) {
				perror("chdir");
				continue;
			}
			found = find_tty(sbuf);
			if (chdir("..") == -1) {
				perror("chdir");
				break;
			}
			if (found != 0) {
				struct name_list *name_list;
				if ((name_list = malloc(sizeof(struct name_list))) == NULL) {
					perror("malloc");
					break;
				}
				name_list->name = strdup(dirent->d_name);
				list_add(&name_list->list, &path_name);
				break;
			}
		}
		if (S_ISCHR(sbuf_cadidate.st_mode)) {
			if (sbuf_cadidate.st_dev == sbuf->st_dev &&
				sbuf_cadidate.st_ino == sbuf->st_ino) {
				struct name_list *name_list;
				if ((name_list = malloc(sizeof(struct name_list))) == NULL) {
					perror("malloc");
					break;
				}
				found = 1;
				name_list->name = strdup(dirent->d_name);
				list_add(&name_list->list, &path_name);
				break;
			}
		}
	}
	closedir(dir);
	return found;
}

char *my_ttyname(int fd)
{
	struct stat sbuf;
	int from;
	int found;
	struct name_list *pos;
	struct name_list *tmp;
	static char path[PATH_MAX] = "/dev";
	int len;
	int i;

	if (isatty(fd) == 0)
		return NULL;

	if (fstat(fd, &sbuf) == -1) {
		perror("fstat");
		return NULL;
	}

	if ((from = open(".", O_RDONLY)) == -1) {
		perror("open");
		return NULL;
	}
	if (chdir("/dev") == -1) {
		perror("chdir");
		close(from);
		return NULL;
	}
	found = find_tty(&sbuf);
	if (fchdir(from) == -1) {
		perror("fchdir");
		close(from);
		return NULL;
	}
	close(from);

	i = strlen(path);
	list_for_each_entry_safe(pos, tmp, &path_name, list) {
		if (found != 0) {
			if ((len = snprintf(path + i, sizeof(path) - i, "/%s", pos->name))
				>= sizeof(path) - i) {
				i = sizeof(path);
			} else {
				i += len;
			}
		}
		free(pos->name);
		free(pos);
	}
	return found != 0 ? path : NULL;
}

int main(int argc, char *argv[])
{
	char *name;
	name = my_ttyname(STDIN_FILENO);
	printf("%s\n", name != NULL ? name : "<NULL>");
	return 0;
}

