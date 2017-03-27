#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(int argc, char *argv[], char *envp[])
{
  int cret = 0;
  printf("%s begins\n", argv[0]);
  pid_t pid = fork();
  switch (pid) {
    case -1: /* error */
      perror("fork");
      return 1;
    case 0:  /* child */
      execl( "/bin/date", "date", "-R", (char*)0 );
      perror("execve");
      return 1;
    default: /* parent */
      if ( wait( &cret ) == -1 ) perror("wait");
      printf("child exits with %d\n", WEXITSTATUS(cret));
  }
  printf("end of main\n");
  exit(EXIT_SUCCESS);
}
