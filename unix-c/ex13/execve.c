#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(int argc, char *argv[], char *envp[])
{
  if (argc >= 2) {
    for (int i = 0; argv[i] != NULL; i++)
      printf("argv[%d]=%s\n",i, argv[i]);
    for (int i = 0; envp[i] != NULL; i++)
      printf("envp[%d]=%s\n",i, envp[i]);
  } else {
    printf("exec itself\n");
    char* const a[] = {argv[0], "a", "b", "c", NULL};
    char* const e[] = {"k1=v1", "k2=v2", NULL};
    execve(argv[0], a, e);
    perror("execve");
  }
  printf("end of main\n");
  exit(EXIT_SUCCESS);
}
