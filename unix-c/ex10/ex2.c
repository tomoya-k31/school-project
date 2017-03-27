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

  printf("%o\n", s.st_mode); /* 8進数が分かりやすい */
  if (s.st_mode&S_IRUSR) printf("owner readable\n");
  if (s.st_mode&S_IWUSR) printf("owner writable\n");
  if (s.st_mode&S_IXUSR) printf("owner execuatble\n");
  if (s.st_mode&S_IRGRP) printf("group readable\n");
  if (s.st_mode&S_IWGRP) printf("group writable\n");
  if (s.st_mode&S_IXGRP) printf("group execuatble\n");
  if (s.st_mode&S_IROTH) printf("other readable\n");
  if (s.st_mode&S_IWOTH) printf("other writable\n");
  if (s.st_mode&S_IXOTH) printf("other execuatble\n");

  return 0;
}
