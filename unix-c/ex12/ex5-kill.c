#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <signal.h>
#include <unistd.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
  if(argc < 2){
    printf("usage: %s pid\n",argv[0]);
    exit(EXIT_FAILURE);
  }

  pid_t pid = atoi(argv[1]);
  if(kill(pid,SIGINT) == -1){
    perror("kill");
    exit(EXIT_FAILURE);
  }

  exit(EXIT_SUCCESS);
}
