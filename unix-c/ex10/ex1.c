#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>

int stat(const char *pathname, struct stat *buf);

int main(int argc, char *argv[])
{
  struct stat s;
  if (argc != 2) {
    fprintf(stderr, "Usage: %s <pathname>\n", argv[0]);
    exit(EXIT_FAILURE);
  }

  if (stat(argv[1], &s) == -1) {
    perror("stat");
    exit(EXIT_FAILURE);
  }

  printf("Last status change:       %s", ctime(&s.st_ctime));
  printf("Last file access:         %s", ctime(&s.st_atime));
  printf("Last file modification:   %s", ctime(&s.st_mtime));

  return 0;
}
