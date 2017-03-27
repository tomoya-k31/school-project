#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <signal.h>
#include <unistd.h>

void func(int sig)
{
  printf("caught SIGINT (%d)\n", sig);
  exit(EXIT_FAILURE);
}

int main(int argc, char *argv[])
{
  printf("pid = %d\n",getpid());
  
  signal(SIGINT, func); /*ハンドラの設定*/
  int x = pause(); /*シグナルが送られるまでスリープ*/
  printf("end of main %d\n", x);
  exit(EXIT_SUCCESS);

  return 0;
}
